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

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

using tcp = boost::asio::ip::tcp;

namespace Infrastructure::Http {

BoostBeastHttpClient::BoostBeastHttpClient(std::chrono::seconds timeout) : timeout_(timeout) {}

std::optional<std::string> BoostBeastHttpClient::get(const std::string& url) {
    return handleRedirect(url, 0);
}

bool BoostBeastHttpClient::isAccessible(const std::string& url) {
    const ParsedUrl parsedUrl = parseUrl(url);
    if (!parsedUrl.valid) {
        return false;
    }

    try {
        int statusCode = 0;

        if (parsedUrl.scheme == "https") {
            auto [body, code] = performHttpsGet(parsedUrl);
            statusCode = code;
        } else {
            auto [body, code] = performHttpGet(parsedUrl);
            statusCode = code;
        }

        // Считаем URL доступным, если статус 2xx или 3xx
        return statusCode >= HTTP_STATUS_OK && statusCode < HTTP_STATUS_BAD_REQUEST;
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
        result.port =
            (result.scheme == "https") ? std::to_string(DEFAULT_HTTPS_PORT) : std::to_string(DEFAULT_HTTP_PORT);
    }

    // Путь (если не указан, используем "/")
    result.path = matches[4].matched ? matches[4].str() : "/";

    result.valid = true;
    return result;
}

std::pair<std::string, int> BoostBeastHttpClient::performHttpGet(const ParsedUrl& parsedUrl) const {
    try {
        // IO context для всех I/O операций
        net::io_context ioc;

        // Резолвим адрес
        tcp::resolver resolver(ioc);
        const auto results = resolver.resolve(parsedUrl.host, parsedUrl.port);

        // Создаём и подключаем сокет
        beast::tcp_stream stream(ioc);
        stream.connect(results);

        // Устанавливаем таймаут
        stream.expires_after(timeout_);

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

        return {res.body(), static_cast<int>(res.result_int())};
    } catch (const std::exception& e) {
        std::cerr << "HTTP ошибка для " << parsedUrl.host << parsedUrl.path << ": " << e.what() << "\n";
        return {"", 0};
    }
}

std::pair<std::string, int> BoostBeastHttpClient::performHttpsGet(const ParsedUrl& parsedUrl) const {
    try {
        // IO context для всех I/O операций
        net::io_context ioc;

        // SSL context
        ssl::context ctx(ssl::context::tlsv12_client);

        // Проверяем сертификаты по умолчанию
        ctx.set_default_verify_paths();
        ctx.set_verify_mode(ssl::verify_peer);

        // Резолвим адрес
        tcp::resolver resolver(ioc);
        const auto results = resolver.resolve(parsedUrl.host, parsedUrl.port);

        // Создаём SSL stream
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        // Set SNI Hostname (для виртуального хостинга)
        if (!SSL_set_tlsext_host_name(stream.native_handle(), parsedUrl.host.c_str())) {
            beast::error_code errc{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw beast::system_error{errc};
        }

        // Подключаемся
        beast::get_lowest_layer(stream).connect(results);

        // Устанавливаем таймаут
        beast::get_lowest_layer(stream).expires_after(timeout_);

        // SSL handshake
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

        return {res.body(), static_cast<int>(res.result_int())};
    } catch (const std::exception& e) {
        std::cerr << "HTTPS ошибка для " << parsedUrl.host << parsedUrl.path << ": " << e.what() << "\n";
        return {"", 0};
    }
}

std::optional<std::string> BoostBeastHttpClient::handleRedirect(const std::string& url, int redirectCount) {
    if (redirectCount >= MAX_REDIRECTS) {
        std::cerr << "Превышено максимальное количество редиректов для " << url << "\n";
        return std::nullopt;
    }

    const ParsedUrl parsedUrl = parseUrl(url);
    if (!parsedUrl.valid) {
        std::cerr << "Некорректный URL: " << url << "\n";
        return std::nullopt;
    }

    try {
        std::string body;
        int statusCode = 0;

        if (parsedUrl.scheme == "https") {
            auto [responseBody, code] = performHttpsGet(parsedUrl);
            body = responseBody;
            statusCode = code;
        } else {
            auto [responseBody, code] = performHttpGet(parsedUrl);
            body = responseBody;
            statusCode = code;
        }

        // Проверяем статус ответа
        if (statusCode >= HTTP_STATUS_OK && statusCode < HTTP_STATUS_MULTIPLE_CHOICES) {
            // Успешный ответ (2xx)
            return body;
        }

        if (statusCode >= HTTP_STATUS_MULTIPLE_CHOICES && statusCode < HTTP_STATUS_BAD_REQUEST) {
            // Редирект (3xx)
            // Нужно извлечь Location заголовок из ответа
            // Для простоты реализации возвращаем текущий body
            // В полной реализации нужно парсить заголовки из response
            std::cerr << "Редирект обнаружен для " << url << ", но обработка Location не реализована\n";
            return body;
        }

        // Ошибка клиента (4xx) или сервера (5xx)
        std::cerr << "HTTP ошибка " << statusCode << " для " << url << "\n";
        return std::nullopt;

    } catch (const std::exception& e) {
        std::cerr << "Исключение при запросе " << url << ": " << e.what() << "\n";
        return std::nullopt;
    }
}

} // namespace Infrastructure::Http
