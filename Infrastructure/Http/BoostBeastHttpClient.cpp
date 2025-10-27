#include "BoostBeastHttpClient.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#include <regex>
#include <sstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

using tcp = boost::asio::ip::tcp;

namespace Infrastructure::Http {
BoostBeastHttpClient::BoostBeastHttpClient(std::chrono::seconds timeout) : timeout_(timeout) {}

void BoostBeastHttpClient::setWorkerId(int workerId) {
    workerId_ = workerId;
}

std::optional<std::string> BoostBeastHttpClient::get(const std::string& url) {
    return handleRedirect(url, 0);
}

bool BoostBeastHttpClient::isAccessible(const std::string& url) {
    const ParsedUrl parsedUrl = parseUrl(url);
    if (!parsedUrl.valid) {
        return false;
    }

    try {
        HttpResponse response;

        if (parsedUrl.scheme == "https") {
            response = performHttpsGet(parsedUrl);
        } else {
            response = performHttpGet(parsedUrl);
        }

        // Считаем URL доступным, если статус 2xx или 3xx
        return response.statusCode >= HTTP_STATUS_OK &&
               response.statusCode < HTTP_STATUS_BAD_REQUEST;
    } catch (const std::exception& e) {
        return false;
    }
}

BoostBeastHttpClient::ParsedUrl BoostBeastHttpClient::parseUrl(const std::string& url) {
    ParsedUrl result;
    result.valid = false;

    // Регулярное выражение для парсинга URL
    // Формат: (http|https)://host(:port)?(/path)?
    const std::regex urlRegex(R"(^(https?):\/\/([^:\/\s]+)(?::(\d+))?(\/.*)?$)");
    std::smatch matches;

    if (!std::regex_match(url, matches, urlRegex)) {
        return result;
    }

    result.scheme = matches[1].str();
    result.host = matches[2].str();

    // Порт (если не указан, используем стандартный)
    if (matches[3].matched) {
        result.port = matches[3].str();
    } else {
        result.port = (result.scheme == "https") ? std::to_string(DEFAULT_HTTPS_PORT)
                                                 : std::to_string(DEFAULT_HTTP_PORT);
    }

    // Путь (если не указан, используем "/")
    result.path = matches[4].matched ? matches[4].str() : "/";

    result.valid = true;
    return result;
}

BoostBeastHttpClient::HttpResponse BoostBeastHttpClient::performHttpGet(
    const ParsedUrl& parsedUrl) const {
    try {
        // IO context для всех I/O операций
        net::io_context ioc;

        // Резолвим адрес
        tcp::resolver resolver(ioc);
        const auto results = resolver.resolve(parsedUrl.host, parsedUrl.port);

        // Создаём сокет и устанавливаем таймаут
        beast::tcp_stream stream(ioc);
        stream.expires_after(timeout_);

        // Подключаемся с таймаутом
        stream.connect(results);

        // Формируем HTTP GET запрос
        http::request<http::string_body> req{http::verb::get, parsedUrl.path, HTTP_VERSION};
        req.set(http::field::host, parsedUrl.host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Отправляем запрос
        http::write(stream, req);

        // Получаем ответ
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        // Закрываем соединение
        beast::error_code errc;
        stream.socket().shutdown(tcp::socket::shutdown_both, errc);

        // Извлекаем заголовок Location (для редиректов)
        HttpResponse response;
        response.body = res.body();
        response.statusCode = static_cast<int>(res.result_int());

        auto locationIt = res.find(http::field::location);
        if (locationIt != res.end()) {
            response.locationHeader = std::string(locationIt->value());
        }

        return response;
    } catch (const std::exception& e) {
        std::cerr << "HTTP ошибка для " << parsedUrl.host << parsedUrl.path << ": " << e.what()
                  << "\n";
        return {"", 0, ""};
    }
}

BoostBeastHttpClient::HttpResponse BoostBeastHttpClient::performHttpsGet(
    const ParsedUrl& parsedUrl) const {
    try {
        // IO context для всех I/O операций
        net::io_context ioc;

        // SSL context
        ssl::context ctx(ssl::context::tlsv12_client);

        // Пробуем загрузить сертификаты разными способами
        try {
            // Попытка 1: загружаем из файла cacert.pem
            ctx.load_verify_file("cacert.pem");
        } catch (const std::exception& e1) {
            try {
                // Попытка 2: системные пути
                ctx.set_default_verify_paths();
            } catch (const std::exception& e2) {
                std::cerr << getLogPrefix()
                          << "Предупреждение: не удалось загрузить SSL сертификаты. "
                          << "Скачайте cacert.pem с https://curl.se/docs/caextract.html\n";
            }
        }

        ctx.set_verify_mode(ssl::verify_peer);

        // Резолвим адрес
        tcp::resolver resolver(ioc);
        const auto results = resolver.resolve(parsedUrl.host, parsedUrl.port);

        // Создаём SSL stream
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        // Устанавливаем таймаут перед любыми сетевыми операциями
        beast::get_lowest_layer(stream).expires_after(timeout_);

        // Set SNI Hostname (для виртуального хостинга)
        if (!SSL_set_tlsext_host_name(stream.native_handle(), parsedUrl.host.c_str())) {
            beast::error_code errc{static_cast<int>(::ERR_get_error()),
                                   net::error::get_ssl_category()};
            throw beast::system_error{errc};
        }

        // Подключаемся с таймаутом
        beast::get_lowest_layer(stream).connect(results);

        // SSL handshake с таймаутом
        stream.handshake(ssl::stream_base::client);

        // Формируем HTTP GET запрос
        http::request<http::string_body> req{http::verb::get, parsedUrl.path, HTTP_VERSION};
        req.set(http::field::host, parsedUrl.host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Отправляем запрос
        http::write(stream, req);

        // Получаем ответ
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        // Закрываем соединение
        beast::error_code errc;
        stream.shutdown(errc);
        // Игнорируем ошибки при закрытии SSL (некоторые серверы закрывают
        // соединение некорректно)

        // Извлекаем заголовок Location (для редиректов)
        HttpResponse response;
        response.body = res.body();
        response.statusCode = static_cast<int>(res.result_int());

        auto locationIt = res.find(http::field::location);
        if (locationIt != res.end()) {
            response.locationHeader = std::string(locationIt->value());
        }

        return response;
    } catch (const std::exception& e) {
        std::cerr << "HTTPS ошибка для " << parsedUrl.host << parsedUrl.path << ": " << e.what()
                  << "\n";
        return {"", 0, ""};
    }
}

std::optional<std::string> BoostBeastHttpClient::handleRedirect(
    const std::string& url,
    int redirectCount,
    std::vector<std::string> visitedUrls) {
    if (redirectCount >= MAX_REDIRECTS) {
        std::cerr << getLogPrefix() << "Превышено максимальное количество редиректов для " << url
                  << "\n";
        return std::nullopt;
    }

    // Проверка на циклические редиректы
    for (const auto& visitedUrl : visitedUrls) {
        if (visitedUrl == url) {
            std::cerr << getLogPrefix() << "Обнаружен циклический редирект: " << url << "\n";
            return std::nullopt;
        }
    }

    // Добавляем текущий URL в список посещённых
    visitedUrls.push_back(url);

    const ParsedUrl parsedUrl = parseUrl(url);
    if (!parsedUrl.valid) {
        std::cerr << getLogPrefix() << "Некорректный URL: " << url << "\n";
        return std::nullopt;
    }

    try {
        HttpResponse response;

        if (parsedUrl.scheme == "https") {
            response = performHttpsGet(parsedUrl);
        } else {
            response = performHttpGet(parsedUrl);
        }

        // Проверяем статус ответа
        if (response.statusCode >= HTTP_STATUS_OK &&
            response.statusCode < HTTP_STATUS_MULTIPLE_CHOICES) {
            // Успешный ответ (2xx)
            return response.body;
        }

        if (response.statusCode >= HTTP_STATUS_MULTIPLE_CHOICES &&
            response.statusCode < HTTP_STATUS_BAD_REQUEST) {
            // Редирект (3xx)
            if (response.locationHeader.empty()) {
                std::cerr << getLogPrefix() << "Редирект обнаружен для " << url
                          << ", но заголовок Location отсутствует\n";
                return response.body;
            }

            // Определяем абсолютный URL для редиректа
            std::string redirectUrl = response.locationHeader;

            // Если Location содержит относительный путь, строим абсолютный URL
            if (redirectUrl.size() >= 2 && redirectUrl[0] == '/' && redirectUrl[1] == '/') {
                // Protocol-relative URL (//example.com)
                redirectUrl = parsedUrl.scheme + ":" + redirectUrl;
            } else if (redirectUrl[0] == '/') {
                // Относительный путь от корня
                redirectUrl = parsedUrl.scheme + "://" + parsedUrl.host + redirectUrl;
            } else if (redirectUrl.substr(0, 4) != "http") {
                // Относительный путь от текущего
                const size_t lastSlash = parsedUrl.path.find_last_of('/');
                const std::string basePath = (lastSlash != std::string::npos)
                                                 ? parsedUrl.path.substr(0, lastSlash + 1)
                                                 : "/";
                redirectUrl = parsedUrl.scheme + "://" + parsedUrl.host + basePath + redirectUrl;
            }

            std::cerr << getLogPrefix() << "Редирект: " << url << " -> " << redirectUrl << "\n";

            // Рекурсивно следуем по редиректу
            return handleRedirect(redirectUrl, redirectCount + 1, visitedUrls);
        }

        // Ошибка клиента (4xx) или сервера (5xx)
        std::cerr << getLogPrefix() << "HTTP ошибка " << response.statusCode << " для " << url
                  << "\n";
        return std::nullopt;

    } catch (const std::exception& e) {
        std::cerr << getLogPrefix() << "Исключение при запросе " << url << ": " << e.what() << "\n";
        return std::nullopt;
    }
}

std::string BoostBeastHttpClient::getLogPrefix() const {
    if (workerId_ == 0) {
        return "";  // Не установлен workerId
    }
    std::ostringstream oss;
    oss << "[Поток " << workerId_ << "] ";
    return oss.str();
}
}  // namespace Infrastructure::Http
