#pragma once

#include "../Model/SearchResult.h"
#include <vector>

namespace Core::Domain::Service {
/**
 * @brief Доменный сервис для ранжирования результатов поиска
 *
 * Отвечает за сортировку и ограничение результатов поиска.
 */
class RankingService {
  public:
    static constexpr size_t DEFAULT_MAX_RESULTS = 10;

    /**
     * @brief Сортирует результаты по убыванию релевантности
     * @param results Результаты поиска
     * @return Отсортированные результаты
     */
    static std::vector<Model::SearchResult> sortByRelevance(
        std::vector<Model::SearchResult> results);

    /**
     * @brief Ограничивает количество результатов
     * @param results Результаты поиска
     * @param maxResults Максимальное количество результатов
     * @return Ограниченный список результатов
     */
    static std::vector<Model::SearchResult> limitResults(std::vector<Model::SearchResult> results,
                                                         size_t maxResults = DEFAULT_MAX_RESULTS);

    /**
     * @brief Сортирует и ограничивает результаты
     * @param results Результаты поиска
     * @param maxResults Максимальное количество результатов
     * @return Отсортированный и ограниченный список результатов
     */
    static std::vector<Model::SearchResult> rankResults(std::vector<Model::SearchResult> results,
                                                        size_t maxResults = DEFAULT_MAX_RESULTS);
};
} // namespace Core::Domain::Service
