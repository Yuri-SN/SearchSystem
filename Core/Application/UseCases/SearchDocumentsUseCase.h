#pragma once

#include <memory>
#include <vector>

#include "../../Domain/Model/SearchResult.h"
#include "../../Domain/Service/RankingService.h"
#include "../../Domain/ValueObject/SearchQuery.h"
#include "../../Ports/ITextProcessor.h"
#include "../../Ports/IWordRepository.h"

namespace Core::Application::UseCases {
/**
 * @brief Use Case для поиска документов
 *
 * Выполняет поиск документов по запросу, ранжирует и ограничивает результаты.
 */
class SearchDocumentsUseCase {
  public:
    /**
     * @brief Конструктор с инъекцией зависимостей
     */
    SearchDocumentsUseCase(std::shared_ptr<Ports::IWordRepository> wordRepository,
                           std::shared_ptr<Ports::ITextProcessor> textProcessor);

    /**
     * @brief Выполняет поиск по запросу
     * @param query Поисковый запрос
     * @param maxResults Максимальное количество результатов (по умолчанию 10)
     * @return Список отранжированных результатов
     */
    std::vector<Domain::Model::SearchResult> execute(
        const Domain::ValueObject::SearchQuery& query,
        size_t maxResults = Domain::Service::RankingService::DEFAULT_MAX_RESULTS);

  private:
    std::shared_ptr<Ports::IWordRepository> wordRepository_;
    std::shared_ptr<Ports::ITextProcessor> textProcessor_;
    Domain::Service::RankingService rankingService_;
};
} // namespace Core::Application::UseCases
