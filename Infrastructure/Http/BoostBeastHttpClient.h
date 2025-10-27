#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "../../Core/Ports/IHttpClient.h"

namespace Infrastructure::Http {
/**
 * @brief Реализация HTTP-клиента на основе Boost.Beast
 *
 * Синхронный HTTP/HTTPS клиент для скачивания веб-страниц.
 * Поддерживает HTTP/1.1, редиректы и базовую обработку ошибок.
 */
class BoostBeastHttpClient : public Core::Ports::IHttpClient {
  public:
    /**
     * @brief Конструктор с настройкой таймаута
     * @param timeout Таймаут для HTTP-запросов
     */
    explicit BoostBeastHttpClient(
        std::chrono::seconds timeout = std::chrono::seconds(HTTP_REQUEST_TIMEOUT_SEC));

    ~BoostBeastHttpClient() override = default;

    /**
     * @brief Выполняет GET-запрос по указанному URL
     * @param url URL для запроса (поддерживает http:// и https://)
     * @return Содержимое страницы, если запрос успешен
     *
     * Поддерживает автоматические редиректы (до 10 раз).
     * Возвращает nullopt в случае ошибки сети или неуспешного HTTP-статуса.
     */
    std::optional<std::string> get(const std::string& url) override;

    /**
     * @brief Проверяет, доступен ли URL
     * @param url URL для проверки
     * @return true если URL доступен (HTTP статус 2xx или 3xx)
     */
    bool isAccessible(const std::string& url) override;

    /**
     * @brief Устанавливает ID рабочего потока для логирования
     * @param workerId Номер потока (1, 2, 3...)
     */
    void setWorkerId(int workerId) override;

  private:
    std::chrono::seconds timeout_;
    int workerId_ = 0;  // ID рабочего потока для логирования

    static constexpr int MAX_REDIRECTS = 5;
    static constexpr int HTTP_VERSION = 11;
    static constexpr int DEFAULT_HTTP_PORT = 80;
    static constexpr int DEFAULT_HTTPS_PORT = 443;
    static constexpr int HTTP_STATUS_OK = 200;
    static constexpr int HTTP_STATUS_MULTIPLE_CHOICES = 300;
    static constexpr int HTTP_STATUS_BAD_REQUEST = 400;
    static constexpr int HTTP_REQUEST_TIMEOUT_SEC = 10;

    /**
     * @brief Структура для хранения распарсенного URL
     */
    struct ParsedUrl {
        std::string scheme;  // http или https
        std::string host;
        std::string port;
        std::string path;
        bool valid;
    };

    /**
     * @brief Структура для хранения HTTP ответа
     */
    struct HttpResponse {
        std::string body;
        int statusCode;
        std::string locationHeader;  // Заголовок Location для редиректов
    };

    /**
     * @brief Парсит URL на составные части
     * @param url URL для парсинга
     * @return Структура с распарсенными компонентами
     */
    static ParsedUrl parseUrl(const std::string& url);

    /**
     * @brief Выполняет HTTP GET-запрос (без SSL)
     * @param parsedUrl Распарсенный URL
     * @return HTTP ответ (тело, статус, заголовки)
     */
    HttpResponse performHttpGet(const ParsedUrl& parsedUrl) const;

    /**
     * @brief Выполняет HTTPS GET-запрос (с SSL)
     * @param parsedUrl Распарсенный URL
     * @return HTTP ответ (тело, статус, заголовки)
     */
    HttpResponse performHttpsGet(const ParsedUrl& parsedUrl) const;

    /**
     * @brief Обрабатывает редиректы
     * @param location URL для редиректа
     * @param redirectCount Текущее количество редиректов
     * @param visitedUrls Список посещённых URL для детектирования циклов
     * @return Содержимое страницы после редиректа
     */
    std::optional<std::string> handleRedirect(const std::string& location,
                                              int redirectCount,
                                              std::vector<std::string> visitedUrls = {});

    /**
     * @brief Формирует префикс лог-сообщения с ID потока
     * @return Строка вида "[Поток X] " для логов
     */
    std::string getLogPrefix() const;
};
} // namespace Infrastructure::Http
