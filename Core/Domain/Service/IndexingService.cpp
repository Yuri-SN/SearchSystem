#include "IndexingService.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace Core::Domain::Service {
std::map<std::string, int> IndexingService::analyzeWordFrequency(const std::string& text) {
    std::map<std::string, int> frequency;

    // Извлекаем слова
    auto words = extractWords(text);

    // Фильтруем по длине
    words = filterWordsByLength(words);

    // Подсчитываем частоту
    for (const auto& word : words) {
        frequency[word]++;
    }

    return frequency;
}

std::vector<std::string> IndexingService::filterWordsByLength(
    const std::vector<std::string>& words) {
    std::vector<std::string> filtered;

    for (const auto& word : words) {
        if (isValidWordLength(word)) {
            filtered.push_back(word);
        }
    }

    return filtered;
}

std::vector<std::string> IndexingService::extractWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream stream(text);
    std::string word;

    while (stream >> word) {
        // Удаляем знаки препинания
        word.erase(std::remove_if(word.begin(), word.end(),
                                  [](unsigned char chr) { return std::ispunct(chr); }),
                   word.end());

        if (!word.empty()) {
            words.push_back(word);
        }
    }

    return words;
}

bool IndexingService::isValidWordLength(const std::string& word) {
    return word.length() >= MIN_WORD_LENGTH && word.length() <= MAX_WORD_LENGTH;
}
} // namespace Core::Domain::Service
