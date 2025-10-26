#include "SearchDocumentsUseCase.h"

namespace Core::Application::UseCases {
SearchDocumentsUseCase::SearchDocumentsUseCase(
    std::shared_ptr<Ports::IWordRepository> wordRepository,
    std::shared_ptr<Ports::ITextProcessor> textProcessor)
    : wordRepository_(std::move(wordRepository)), textProcessor_(std::move(textProcessor)) {}

std::vector<Domain::Model::SearchResult> SearchDocumentsUseCase::execute(
    const Domain::ValueObject::SearchQuery& query,
    size_t maxResults) {
    // Нормализуем и приводим термы запроса к нижнему регистру
    // (так же, как при индексации документов)
    std::vector<std::string> terms;
    for (const auto& term : query.getTerms()) {
        std::string normalized = textProcessor_->normalize(term);
        terms.push_back(textProcessor_->toLowercase(normalized));
    }

    // Ищем документы
    auto results = wordRepository_->search(terms);

    // Ранжируем и ограничиваем результаты
    results = Core::Domain::Service::RankingService::rankResults(std::move(results), maxResults);

    return results;
}
} // namespace Core::Application::UseCases
