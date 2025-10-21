#pragma once

#include <memory>
#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "../../Core/Ports/IHttpServer.h"

namespace Infrastructure::Http {

/**
 * @brief Асинхронный HTTP-сервер на основе Boost.Beast и Boost.Asio
 *
 * Многопоточный HTTP/1.1 сервер для REST API поисковой системы.
 * Использует пул потоков для обработки запросов.
 */
class BoostBeastHttpServer : public Core::Ports::IHttpServer {
  public:
    /**
     * @brief Конструктор с настройкой количества потоков
     * @param threadCount Количество потоков для обработки запросов (по умолчанию
     * 4)
     */
    explicit BoostBeastHttpServer(unsigned int threadCount = 4);

    ~BoostBeastHttpServer() override;

    /**
     * @brief Запускает HTTP-сервер
     * @param port Порт для прослушивания
     * @param handler Обработчик HTTP-запросов
     *
     * Сервер начинает принимать соединения на указанном порту.
     * Блокирует вызывающий поток до вызова stop().
     */
    void start(int port, RequestHandler handler) override;

    /**
     * @brief Останавливает HTTP-сервер
     *
     * Останавливает прием новых соединений и завершает работу всех потоков.
     */
    void stop() override;

  private:
    unsigned int threadCount_;
    std::unique_ptr<boost::asio::io_context> ioc_;
    std::vector<std::thread> threads_;
    RequestHandler handler_;

    static constexpr int BACKLOG_SIZE = 128;

    /**
     * @brief Запускает прием соединений
     * @param acceptor TCP acceptor для принятия соединений
     */
    void doAccept(const std::shared_ptr<boost::asio::ip::tcp::acceptor>& acceptor);

    /**
     * @brief Обрабатывает одно соединение
     * @param socket TCP сокет для соединения
     */
    void handleSession(boost::asio::ip::tcp::socket socket);
};

} // namespace Infrastructure::Http
