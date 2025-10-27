#include "HtmlParser.h"

#include <gumbo.h>
#include <algorithm>
#include <cctype>

namespace Infrastructure::Parsers {

std::string HtmlParser::extractText(const std::string& html) {
    // Парсим HTML с помощью Gumbo
    GumboOutput* output = gumbo_parse(html.c_str());

    std::string text;
    extractTextFromNode(output->root, text);

    // Освобождаем память
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    return text;
}

void HtmlParser::extractTextFromNode(void* nodePtr, std::string& text) {
    GumboNode* node = static_cast<GumboNode*>(nodePtr);

    if (node->type == GUMBO_NODE_TEXT) {
        // Это текстовый узел - добавляем его содержимое
        text += node->v.text.text;
        text += " ";
    } else if (node->type == GUMBO_NODE_ELEMENT) {
        // Пропускаем скрипты и стили
        GumboElement* element = &node->v.element;
        if (element->tag == GUMBO_TAG_SCRIPT || element->tag == GUMBO_TAG_STYLE) {
            return;
        }

        // Рекурсивно обрабатываем дочерние узлы
        GumboVector* children = &element->children;
        for (unsigned int i = 0; i < children->length; ++i) {
            extractTextFromNode(children->data[i], text);
        }
    }
}

std::vector<std::string> HtmlParser::extractLinks(const std::string& html, const std::string& baseUrl) {
    // Парсим HTML с помощью Gumbo
    GumboOutput* output = gumbo_parse(html.c_str());

    std::vector<std::string> links;
    extractLinksFromNode(output->root, links, baseUrl);

    // Освобождаем память
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    return links;
}

void HtmlParser::extractLinksFromNode(void* nodePtr, std::vector<std::string>& links, const std::string& baseUrl) {
    GumboNode* node = static_cast<GumboNode*>(nodePtr);

    if (node->type == GUMBO_NODE_ELEMENT) {
        GumboElement* element = &node->v.element;

        // Ищем тег <a> с атрибутом href
        if (element->tag == GUMBO_TAG_A) {
            GumboAttribute* href = gumbo_get_attribute(&element->attributes, "href");
            if (href) {
                std::string url = href->value;

                // Пропускаем якоря и javascript ссылки
                if (url.empty() || url[0] == '#' || url.find("javascript:") == 0 || url.find("mailto:") == 0) {
                    // Пропускаем
                } else {
                    // Преобразуем относительный URL в абсолютный
                    if (!isAbsoluteUrl(url)) {
                        url = resolveUrl(url, baseUrl);
                    }
                    links.push_back(url);
                }
            }
        }

        // Рекурсивно обрабатываем дочерние узлы
        GumboVector* children = &element->children;
        for (unsigned int i = 0; i < children->length; ++i) {
            extractLinksFromNode(children->data[i], links, baseUrl);
        }
    }
}

bool HtmlParser::isAbsoluteUrl(const std::string& url) {
    // Проверяем наличие схемы (http:// или https://)
    return url.find("http://") == 0 || url.find("https://") == 0;
}

std::string HtmlParser::resolveUrl(const std::string& relativeUrl, const std::string& baseUrl) {
    // Простая реализация разрешения относительных URL
    // Для полноценной реализации лучше использовать библиотеку

    if (relativeUrl.empty()) {
        return baseUrl;
    }

    // Если начинается с //, это protocol-relative URL
    if (relativeUrl.size() >= 2 && relativeUrl[0] == '/' && relativeUrl[1] == '/') {
        // Извлекаем только схему из baseUrl
        size_t schemeEnd = baseUrl.find("://");
        if (schemeEnd != std::string::npos) {
            return baseUrl.substr(0, schemeEnd + 1) + relativeUrl;
        }
        return "https:" + relativeUrl;
    }

    // Если начинается с /, это путь от корня домена
    if (relativeUrl[0] == '/') {
        // Извлекаем схему и хост из baseUrl
        size_t schemeEnd = baseUrl.find("://");
        if (schemeEnd != std::string::npos) {
            size_t hostEnd = baseUrl.find('/', schemeEnd + 3);
            std::string baseHost;
            if (hostEnd != std::string::npos) {
                baseHost = baseUrl.substr(0, hostEnd);
            } else {
                baseHost = baseUrl;
            }
            return baseHost + relativeUrl;
        }
    }

    // Если относительный путь (без /), добавляем к текущему каталогу
    size_t lastSlash = baseUrl.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return baseUrl.substr(0, lastSlash + 1) + relativeUrl;
    }

    return baseUrl + "/" + relativeUrl;
}

} // namespace Infrastructure::Parsers
