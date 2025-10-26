#pragma once

#include <cstdint>

namespace Core::Domain::Model {
/**
 * @brief Связь между документом и словом с частотой встречаемости
 *
 * Представляет запись в промежуточной таблице для связи "многие-ко-многим"
 * между документами и словами. Хранит частоту встречаемости слова в документе.
 */
class WordFrequency {
  public:
    using DocumentIdType = int64_t;
    using WordIdType = int64_t;
    using FrequencyType = int32_t;

    /**
     * @brief Конструктор
     * @param documentId ID документа
     * @param wordId ID слова
     * @param frequency Частота встречаемости слова в документе
     */
    WordFrequency(DocumentIdType documentId, WordIdType wordId, FrequencyType frequency);

    // Геттеры
    DocumentIdType getDocumentId() const;
    WordIdType getWordId() const;
    FrequencyType getFrequency() const;

    // Сеттер для частоты (если нужно обновить)
    void setFrequency(FrequencyType frequency);

  private:
    DocumentIdType documentId_;
    WordIdType wordId_;
    FrequencyType frequency_;
};
} // namespace Core::Domain::Model
