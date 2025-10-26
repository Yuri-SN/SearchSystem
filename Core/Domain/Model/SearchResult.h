#pragma once

#include <cstdint>
#include <string>

namespace Core::Domain::Model {
/**
 * @brief Результат поиска - документ с его релевантностью
 *
 * Содержит информацию о документе и его релевантности (сумма частот искомых
 * слов)
 */
class SearchResult {
  public:
    using DocumentIdType = int64_t;
    using RelevanceType = int32_t;

    /**
     * @brief Конструктор
     * @param documentId ID документа
     * @param url URL документа
     * @param relevance Релевантность (сумма частот искомых слов в документе)
     */
    SearchResult(DocumentIdType documentId, std::string url, RelevanceType relevance);

    // Геттеры
    DocumentIdType getDocumentId() const;
    const std::string& getUrl() const;
    RelevanceType getRelevance() const;

    // Оператор сравнения для сортировки по релевантности (по убыванию)
    bool operator<(const SearchResult& other) const;
    bool operator>(const SearchResult& other) const;

  private:
    DocumentIdType documentId_;
    std::string url_;
    RelevanceType relevance_;
};
} // namespace Core::Domain::Model
