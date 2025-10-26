#pragma once

#include <optional>
#include <string>

namespace Core::Ports {
/**
 * @brief Интерфейс HTTP-клиента для скачивания веб-страниц
 *
 * Порт для HTTP-запросов.
 * Реализация будет в Infrastructure слое (Boost Beast).
 */
class IHttpClient {
  public:
    virtual ~IHttpClient() = default;

    /**
     * @brief Выполняет GET-запрос по указанному URL
     * @param url URL для запроса
     * @return Содержимое страницы, если запрос успешен
     */
    virtual std::optional<std::string> get(const std::string& url) = 0;

    /**
     * @brief Проверяет, доступен ли URL
     * @param url URL для проверки
     * @return true если URL доступен
     */
    virtual bool isAccessible(const std::string& url) = 0;

    /**
     * @brief Устанавливает ID рабочего потока для логирования
     * @param workerId Номер потока (1, 2, 3...)
     */
    virtual void setWorkerId(int workerId) = 0;
};
} // namespace Core::Ports
