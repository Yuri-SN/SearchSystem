#include "RankingService.h"

#include <algorithm>

namespace Core::Domain::Service {
std::vector<Model::SearchResult> RankingService::sortByRelevance(
    std::vector<Model::SearchResult> results) {
    // Сортируем по убыванию релевантности
    std::sort(results.begin(), results.end());
    return results;
}

std::vector<Model::SearchResult> RankingService::limitResults(
    std::vector<Model::SearchResult> results,
    size_t maxResults) {
    if (results.size() > maxResults) {
        results.erase(results.begin() + static_cast<ptrdiff_t>(maxResults), results.end());
    }
    return results;
}

std::vector<Model::SearchResult> RankingService::rankResults(
    std::vector<Model::SearchResult> results,
    size_t maxResults) {
    results = sortByRelevance(std::move(results));
    results = limitResults(std::move(results), maxResults);
    return results;
}
} // namespace Core::Domain::Service
