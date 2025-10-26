#pragma once

#include <functional>
#include <map>
#include <string>

namespace Core::Ports {
/**
 * @brief HTTP-ответ с телом и заголовками
 */
struct HttpResponse {
    std::string body;
    std::map<std::string, std::string> headers;
    int statusCode = 200;

    /**
     * @brief Создаёт HTML-ответ
     */
    static HttpResponse html(const std::string& htmlBody, int status = 200) {
        HttpResponse response;
        response.body = htmlBody;
        response.statusCode = status;
        response.headers["Content-Type"] = "text/html; charset=utf-8";
        return response;
    }

    /**
     * @brief Создаёт JSON-ответ
     */
    static HttpResponse json(const std::string& jsonBody, int status = 200) {
        HttpResponse response;
        response.body = jsonBody;
        response.statusCode = status;
        response.headers["Content-Type"] = "application/json";
        return response;
    }

    /**
     * @brief Создаёт текстовый ответ
     */
    static HttpResponse text(const std::string& textBody, int status = 200) {
        HttpResponse response;
        response.body = textBody;
        response.statusCode = status;
        response.headers["Content-Type"] = "text/plain; charset=utf-8";
        return response;
    }
};

/**
 * @brief Интерфейс HTTP-сервера
 *
 * Порт для HTTP-сервера поисковой системы.
 * Реализация будет в Infrastructure слое (Boost Beast).
 */
class IHttpServer {
  public:
    using RequestHandler = std::function<HttpResponse(const std::string& method,
                                                      const std::string& target,
                                                      const std::string& body)>;

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
