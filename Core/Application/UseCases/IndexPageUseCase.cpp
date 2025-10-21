#include "IndexPageUseCase.h"

namespace Core::Application::UseCases {

IndexPageUseCase::IndexPageUseCase(std::shared_ptr<Ports::IDocumentRepository> documentRepository,
                                   std::shared_ptr<Ports::IWordRepository> wordRepository,
                                   std::shared_ptr<Ports::IHtmlParser> htmlParser,
                                   std::shared_ptr<Ports::ITextProcessor> textProcessor)
    : documentRepository_(std::move(documentRepository)),
      wordRepository_(std::move(wordRepository)),
      htmlParser_(std::move(htmlParser)),
      textProcessor_(std::move(textProcessor)) {}

Domain::Model::Document::IdType IndexPageUseCase::execute(const std::string& url, const std::string& htmlContent) {
    // Проверяем, не проиндексирован ли уже этот документ
    if (documentRepository_->existsByUrl(url)) {
        return 0;  // Документ уже существует
    }

    // Извлекаем текст из HTML
    std::string text = htmlParser_->extractText(htmlContent);

    // Нормализуем текст
    text = textProcessor_->normalize(text);

    // Приводим к нижнему регистру
    text = textProcessor_->toLowercase(text);

    // Анализируем частотность слов
    auto wordFrequencies = Core::Domain::Service::IndexingService::analyzeWordFrequency(text);

    // Сохраняем документ
    Domain::Model::Document document(url, text);
    auto documentId = documentRepository_->save(document);

    // Сохраняем частотность слов
    wordRepository_->saveWordFrequencies(documentId, wordFrequencies);

    return documentId;
}

} // namespace Core::Application::UseCases
