#pragma once

#include <map>
#include <string>
#include <vector>

namespace Core::Domain::Service {
/**
 * @brief Доменный сервис для индексации текста
 *
 * Отвечает за обработку текста: очистку от HTML, анализ частотности слов.
 * Чистая бизнес-логика без зависимостей от инфраструктуры.
 */
class IndexingService {
  public:
    /**
     * @brief Анализирует частотность слов в тексте
     * @param text Очищенный текст (без HTML-тегов)
     * @return Карта слово -> частота встречаемости
     */
    static std::map<std::string, int> analyzeWordFrequency(const std::string& text);

    /**
     * @brief Фильтрует слова по длине (от 3 до 32 символов)
     * @param words Список слов
     * @return Отфильтрованный список слов
     */
    static std::vector<std::string> filterWordsByLength(const std::vector<std::string>& words);

    /**
     * @brief Извлекает слова из текста
     * @param text Текст
     * @return Список слов
     */
    static std::vector<std::string> extractWords(const std::string& text);

  private:
    static constexpr size_t MIN_WORD_LENGTH = 3;
    static constexpr size_t MAX_WORD_LENGTH = 32;

    /**
     * @brief Проверяет, соответствует ли слово требованиям по длине
     */
    static bool isValidWordLength(const std::string& word);
};
} // namespace Core::Domain::Service
