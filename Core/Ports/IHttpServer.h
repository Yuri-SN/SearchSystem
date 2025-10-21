#pragma once

#include <functional>
#include <string>

namespace Core::Ports {

/**
 * @brief Интерфейс HTTP-сервера
 *
 * Порт для HTTP-сервера поисковой системы.
 * Реализация будет в Infrastructure слое (Boost Beast).
 */
class IHttpServer {
  public:
    using RequestHandler =
        std::function<std::string(const std::string& method, const std::string& target, const std::string& body)>;

    virtual ~IHttpServer() = default;

    /**
     * @brief Запускает HTTP-сервер
     * @param port Порт для прослушивания
     * @param handler Обработчик HTTP-запросов
     */
    virtual void start(int port, RequestHandler handler) = 0;

    /**
     * @brief Останавливает HTTP-сервер
     */
    virtual void stop() = 0;
};

} // namespace Core::Ports
