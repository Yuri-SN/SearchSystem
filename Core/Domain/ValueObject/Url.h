#pragma once

#include <optional>
#include <string>

namespace Core::Domain::ValueObject {
/**
 * @brief Value Object представляющий URL
 *
 * Валидирует и парсит URL в формате: scheme://host/path
 * Поддерживает схемы http и https.
 */
class Url {
  public:
    /**
     * @brief Создает URL из строки
     * @param url Строка URL для парсинга
     * @return Объект Url если валидация успешна, иначе std::nullopt
     */
    static std::optional<Url> create(const std::string& url);

    /**
     * @brief Получить полную строку URL
     */
    const std::string& toString() const;

    /**
     * @brief Получить схему URL (http или https)
     */
    const std::string& getScheme() const;

    /**
     * @brief Получить хост URL
     */
    const std::string& getHost() const;

    /**
     * @brief Получить путь URL
     */
    const std::string& getPath() const;

    bool operator==(const Url& other) const;
    bool operator!=(const Url& other) const;

  private:
    /**
     * @brief Приватный конструктор (используется фабричным методом create)
     */
    Url(std::string url, std::string scheme, std::string host, std::string path);

    /**
     * @brief Парсинг и валидация URL
     */
    static bool parse(const std::string& url,
                      std::string& scheme,
                      std::string& host,
                      std::string& path);

    std::string url_;
    std::string scheme_;
    std::string host_;
    std::string path_;
};
} // namespace Core::Domain::ValueObject
