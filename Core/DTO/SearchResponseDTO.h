#pragma once

#include <string>
#include <vector>

#include "../Domain/Model/SearchResult.h"

namespace Core::DTO {
/**
 * @brief DTO для ответа на поисковый запрос
 */
struct SearchResponseDTO {
    std::vector<Domain::Model::SearchResult> results;  // Результаты поиска
    bool success{true};                                // Успешность выполнения запроса
    std::string errorMessage;                          // Сообщение об ошибке (если есть)
    int totalFound{0};                                 // Общее количество найденных документов
};
} // namespace Core::DTO
