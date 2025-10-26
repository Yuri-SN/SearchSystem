#include "SearchQuery.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace Core::Domain::ValueObject {
std::optional<SearchQuery> SearchQuery::create(const std::string& query) {
    std::vector<std::string> terms;

    if (!parse(query, terms)) {
        return std::nullopt;
    }

    return SearchQuery(query, std::move(terms));
}

const std::vector<std::string>& SearchQuery::getTerms() const {
    return terms_;
}

const std::string& SearchQuery::toString() const {
    return query_;
}

bool SearchQuery::isEmpty() const {
    return terms_.empty();
}

SearchQuery::SearchQuery(std::string query, std::vector<std::string> terms)
    : query_(std::move(query)), terms_(std::move(terms)) {}

bool SearchQuery::parse(const std::string& query, std::vector<std::string>& terms) {
    std::istringstream stream(query);
    std::string term;

    // Разбиваем по пробелам
    while (stream >> term) {
        // Удаляем знаки препинания
        term.erase(std::remove_if(term.begin(), term.end(),
                                  [](unsigned char chr) { return std::ispunct(chr); }),
                   term.end());

        // Пропускаем пустые термы
        if (term.empty()) {
            continue;
        }

        terms.push_back(term);

        // Проверка ограничения на количество слов
        if (terms.size() > MAX_TERMS) {
            return false;
        }
    }

    // Запрос не должен быть пустым
    return !terms.empty();
}
} // namespace Core::Domain::ValueObject
