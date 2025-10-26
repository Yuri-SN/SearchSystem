#pragma once

#include <string>

namespace Core::DTO {
/**
 * @brief DTO для поискового запроса
 */
struct SearchRequestDTO {
    static constexpr int DEFAULT_MAX_RESULTS = 10;

    std::string query;                    // Строка поискового запроса
    int maxResults{DEFAULT_MAX_RESULTS};  // Максимальное количество результатов
};
} // namespace Core::DTO
