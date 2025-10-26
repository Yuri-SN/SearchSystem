#pragma once

#include <string>
#include <vector>

namespace Core::DTO {
/**
 * @brief DTO для результата краулинга страницы
 */
struct CrawlResultDTO {
    std::string url;                 // URL страницы
    std::string content;             // HTML-содержимое страницы
    std::vector<std::string> links;  // Найденные ссылки на странице
    bool success{false};             // Успешность краулинга
    std::string errorMessage;        // Сообщение об ошибке (если есть)
};
} // namespace Core::DTO
