#include "SearchResult.h"

namespace Core::Domain::Model {
SearchResult::SearchResult(DocumentIdType documentId, std::string url, RelevanceType relevance)
    : documentId_(documentId), url_(std::move(url)), relevance_(relevance) {}

SearchResult::DocumentIdType SearchResult::getDocumentId() const {
    return documentId_;
}

const std::string& SearchResult::getUrl() const {
    return url_;
}

SearchResult::RelevanceType SearchResult::getRelevance() const {
    return relevance_;
}

bool SearchResult::operator<(const SearchResult& other) const {
    // Для сортировки по убыванию релевантности
    return relevance_ > other.relevance_;
}

bool SearchResult::operator>(const SearchResult& other) const {
    return relevance_ < other.relevance_;
}
} // namespace Core::Domain::Model
