#include "BoostBeastHttpServer.h"

#include <iostream>

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace Infrastructure::Http {

BoostBeastHttpServer::BoostBeastHttpServer(unsigned int threadCount) : threadCount_(threadCount), ioc_(nullptr) {}

BoostBeastHttpServer::~BoostBeastHttpServer() {
    stop();
}

void BoostBeastHttpServer::start(int port, RequestHandler handler) {
    handler_ = std::move(handler);

    // Создаём io_context
    ioc_ = std::make_unique<net::io_context>(static_cast<int>(threadCount_));

    // Создаём acceptor
    auto acceptor = std::make_shared<tcp::acceptor>(*ioc_);
    const tcp::endpoint endpoint{tcp::v4(), static_cast<unsigned short>(port)};

    beast::error_code errc;

    // Открываем acceptor
    acceptor->open(endpoint.protocol(), errc);
    if (errc) {
        std::cerr << "Ошибка открытия acceptor: " << errc.message() << "\n";
        return;
    }

    // Устанавливаем SO_REUSEADDR
    acceptor->set_option(net::socket_base::reuse_address(true), errc);
    if (errc) {
        std::cerr << "Ошибка установки опции reuse_address: " << errc.message() << "\n";
        return;
    }

    // Привязываем к адресу
    acceptor->bind(endpoint, errc);
    if (errc) {
        std::cerr << "Ошибка привязки к порту " << port << ": " << errc.message() << "\n";
        return;
    }

    // Начинаем прослушивание
    acceptor->listen(BACKLOG_SIZE, errc);
    if (errc) {
        std::cerr << "Ошибка начала прослушивания: " << errc.message() << "\n";
        return;
    }

    std::cout << "HTTP сервер запущен на порту " << port << "\n";

    // Начинаем принимать соединения
    doAccept(acceptor);

    // Запускаем пул потоков для io_context
    threads_.reserve(threadCount_);
    for (unsigned int i = 0; i < threadCount_; ++i) {
        threads_.emplace_back([this] { ioc_->run(); });
    }

    // Ждём завершения всех потоков
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void BoostBeastHttpServer::stop() {
    if (ioc_) {
        ioc_->stop();
    }

    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    threads_.clear();
    std::cout << "HTTP сервер остановлен\n";
}

void BoostBeastHttpServer::doAccept(const std::shared_ptr<tcp::acceptor>& acceptor) {
    acceptor->async_accept(net::make_strand(*ioc_), [this, acceptor](beast::error_code errc, tcp::socket socket) {
        if (!errc) {
            // Обрабатываем соединение в отдельной задаче
            net::post(socket.get_executor(),
                      [this, sck = std::move(socket)]() mutable { handleSession(std::move(sck)); });
        } else {
            std::cerr << "Ошибка принятия соединения: " << errc.message() << "\n";
        }

        // Принимаем следующее соединение
        doAccept(acceptor);
    });
}

void BoostBeastHttpServer::handleSession(tcp::socket socket) {
    beast::error_code errc;

    // Буфер для чтения
    beast::flat_buffer buffer;

    // Читаем HTTP-запрос
    http::request<http::string_body> req;
    http::read(socket, buffer, req, errc);

    if (errc == http::error::end_of_stream) {
        // Клиент закрыл соединение
        socket.shutdown(tcp::socket::shutdown_send, errc);
        return;
    }

    if (errc) {
        std::cerr << "Ошибка чтения запроса: " << errc.message() << "\n";
        return;
    }

    // Вызываем обработчик запроса
    std::string responseBody;
    http::status responseStatus = http::status::ok;

    try {
        responseBody = handler_(std::string(req.method_string()), std::string(req.target()), req.body());
    } catch (const std::exception& e) {
        responseBody = R"({"error": "Internal server error"})";
        responseStatus = http::status::internal_server_error;
        std::cerr << "Ошибка обработки запроса: " << e.what() << "\n";
    }

    // Создаём HTTP-ответ
    http::response<http::string_body> res{responseStatus, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = responseBody;
    res.prepare_payload();

    // Отправляем ответ
    http::write(socket, res, errc);

    if (errc) {
        std::cerr << "Ошибка отправки ответа: " << errc.message() << "\n";
        return;
    }

    // Закрываем соединение, если не keep-alive
    if (!req.keep_alive()) {
        socket.shutdown(tcp::socket::shutdown_send, errc);
    }
}

} // namespace Infrastructure::Http
