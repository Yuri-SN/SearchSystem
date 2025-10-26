#include "IndexPageUseCase.h"

#include <stdexcept>

namespace Core::Application::UseCases {
IndexPageUseCase::IndexPageUseCase(std::shared_ptr<Ports::IDocumentRepository> documentRepository,
                                   std::shared_ptr<Ports::IWordRepository> wordRepository,
                                   std::shared_ptr<Ports::IHtmlParser> htmlParser,
                                   std::shared_ptr<Ports::ITextProcessor> textProcessor)
    : documentRepository_(std::move(documentRepository)),
      wordRepository_(std::move(wordRepository)),
      htmlParser_(std::move(htmlParser)),
      textProcessor_(std::move(textProcessor)) {}

Domain::Model::Document::IdType IndexPageUseCase::execute(const std::string& url,
                                                          const std::string& htmlContent) {
    // Извлекаем текст из HTML
    std::string text = htmlParser_->extractText(htmlContent);

    // Нормализуем текст
    text = textProcessor_->normalize(text);

    // Приводим к нижнему регистру
    text = textProcessor_->toLowercase(text);

    // Анализируем частотность слов
    auto wordFrequencies = Core::Domain::Service::IndexingService::analyzeWordFrequency(text);

    // Выполняем все операции с БД в одной транзакции
    // Это критично для многопоточной работы
    Domain::Model::Document document(url, text);
    Domain::Model::Document::IdType documentId = 0;

    try {
        // Сохраняем документ (создаём новый или обновляем существующий)
        documentId = documentRepository_->save(document);

        // Сохраняем частотность слов (обновляем существующие или создаём новые)
        wordRepository_->saveWordFrequencies(documentId, wordFrequencies);
    } catch (const std::exception& e) {
        // Перебрасываем исключение с дополнительной информацией
        throw std::runtime_error("Ошибка при индексации страницы " + url + ": " + e.what());
    }

    return documentId;
}
} // namespace Core::Application::UseCases
