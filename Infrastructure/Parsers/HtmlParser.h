#pragma once

#include "../../Core/Ports/IHtmlParser.h"
#include <string>
#include <vector>

namespace Infrastructure::Parsers {

/**
 * @brief Реализация IHtmlParser с использованием gumbo-parser
 *
 * Использует библиотеку Gumbo от Google для надёжного парсинга HTML.
 */
class HtmlParser : public Core::Ports::IHtmlParser {
  public:
    HtmlParser() = default;
    ~HtmlParser() override = default;

    /**
     * @brief Извлекает текст из HTML (удаляет теги)
     * @param html HTML-контент
     * @return Чистый текст без HTML-тегов
     */
    std::string extractText(const std::string& html) override;

    /**
     * @brief Извлекает все ссылки из HTML
     * @param html HTML-контент
     * @param baseUrl Базовый URL для разрешения относительных ссылок
     * @return Список найденных URL
     */
    std::vector<std::string> extractLinks(const std::string& html, const std::string& baseUrl) override;

  private:
    /**
     * @brief Рекурсивно извлекает текст из узла Gumbo
     */
    void extractTextFromNode(void* node, std::string& text);

    /**
     * @brief Рекурсивно извлекает ссылки из узла Gumbo
     */
    void extractLinksFromNode(void* node, std::vector<std::string>& links, const std::string& baseUrl);

    /**
     * @brief Преобразует относительный URL в абсолютный
     */
    std::string resolveUrl(const std::string& relativeUrl, const std::string& baseUrl);

    /**
     * @brief Проверяет, является ли URL абсолютным
     */
    bool isAbsoluteUrl(const std::string& url);
};

} // namespace Infrastructure::Parsers
