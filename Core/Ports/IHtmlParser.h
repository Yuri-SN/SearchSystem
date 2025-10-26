#pragma once

#include <string>
#include <vector>

namespace Core::Ports {
/**
 * @brief Интерфейс парсера HTML
 *
 * Порт для парсинга HTML-страниц.
 * Реализация будет в Infrastructure слое.
 */
class IHtmlParser {
  public:
    virtual ~IHtmlParser() = default;

    /**
     * @brief Извлекает текст из HTML (удаляет теги)
     * @param html HTML-контент
     * @return Чистый текст без HTML-тегов
     */
    virtual std::string extractText(const std::string& html) = 0;

    /**
     * @brief Извлекает все ссылки из HTML
     * @param html HTML-контент
     * @param baseUrl Базовый URL для разрешения относительных ссылок
     * @return Список найденных URL
     */
    virtual std::vector<std::string> extractLinks(const std::string& html,
                                                  const std::string& baseUrl) = 0;
};
} // namespace Core::Ports
