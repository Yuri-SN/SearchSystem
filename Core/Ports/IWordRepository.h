#pragma once

#include <map>
#include <optional>
#include <vector>

#include "../Domain/Model/Document.h"
#include "../Domain/Model/SearchResult.h"
#include "../Domain/Model/Word.h"
#include "../Domain/Model/WordFrequency.h"

namespace Core::Ports {
/**
 * @brief Интерфейс репозитория для работы со словами и частотностью
 *
 * Порт для доступа к хранилищу слов и их частотности в документах.
 * Реализация будет в Infrastructure слое.
 */
class IWordRepository {
  public:
    virtual ~IWordRepository() = default;

    /**
     * @brief Сохраняет слово в хранилище
     * @param word Слово для сохранения
     * @return ID сохраненного слова
     */
    virtual Domain::Model::Word::IdType save(Domain::Model::Word& word) = 0;

    /**
     * @brief Находит слово по тексту
     * @param text Текст слова
     * @return Слово, если найдено
     */
    virtual std::optional<Domain::Model::Word> findByText(const std::string& text) = 0;

    /**
     * @brief Сохраняет частотность слова в документе
     * @param frequency Частотность для сохранения
     */
    virtual void saveFrequency(const Domain::Model::WordFrequency& frequency) = 0;

    /**
     * @brief Сохраняет несколько записей о частотности слов
     * @param documentId ID документа
     * @param wordFrequencies Карта: слово -> частота
     */
    virtual void saveWordFrequencies(Domain::Model::Document::IdType documentId,
                                     const std::map<std::string, int>& wordFrequencies) = 0;

    /**
     * @brief Ищет документы, содержащие все указанные слова
     * @param words Список слов для поиска
     * @return Список результатов поиска с релевантностью
     */
    virtual std::vector<Domain::Model::SearchResult> search(
        const std::vector<std::string>& words) = 0;
};
} // namespace Core::Ports
