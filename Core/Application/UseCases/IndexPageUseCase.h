#pragma once

#include <memory>
#include <string>

#include "../../Domain/Service/IndexingService.h"
#include "../../Ports/IDocumentRepository.h"
#include "../../Ports/IHtmlParser.h"
#include "../../Ports/ITextProcessor.h"
#include "../../Ports/IWordRepository.h"

namespace Core::Application::UseCases {
/**
 * @brief Use Case для индексации веб-страницы
 *
 * Выполняет полную индексацию страницы: парсинг HTML, извлечение текста,
 * анализ частотности слов и сохранение в БД.
 */
class IndexPageUseCase {
  public:
    /**
     * @brief Конструктор с инъекцией зависимостей
     */
    IndexPageUseCase(std::shared_ptr<Ports::IDocumentRepository> documentRepository,
                     std::shared_ptr<Ports::IWordRepository> wordRepository,
                     std::shared_ptr<Ports::IHtmlParser> htmlParser,
                     std::shared_ptr<Ports::ITextProcessor> textProcessor);

    /**
     * @brief Индексирует веб-страницу
     * @param url URL страницы
     * @param htmlContent HTML-содержимое страницы
     * @return ID созданного документа, или 0 если индексация не удалась
     */
    Domain::Model::Document::IdType execute(const std::string& url, const std::string& htmlContent);

  private:
    std::shared_ptr<Ports::IDocumentRepository> documentRepository_;
    std::shared_ptr<Ports::IWordRepository> wordRepository_;
    std::shared_ptr<Ports::IHtmlParser> htmlParser_;
    std::shared_ptr<Ports::ITextProcessor> textProcessor_;
    Domain::Service::IndexingService indexingService_;
};
} // namespace Core::Application::UseCases
