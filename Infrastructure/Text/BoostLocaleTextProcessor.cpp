#include "BoostLocaleTextProcessor.h"

namespace Infrastructure::Text {

BoostLocaleTextProcessor::BoostLocaleTextProcessor(const std::string& localeName) {
    // Генерируем локаль
    gen_.locale_cache_enabled(true);
    locale_ = gen_.generate(localeName);

    // Устанавливаем глобальную локаль для корректной работы с UTF-8
    std::locale::global(locale_);
}

std::string BoostLocaleTextProcessor::toLowercase(const std::string& text) {
    // Используем Boost.Locale для корректного приведения к нижнему регистру
    // с учётом правил локали (например, турецкий I -> ı, русский Ё -> ё)
    return boost::locale::to_lower(text, locale_);
}

std::string BoostLocaleTextProcessor::normalize(const std::string& text) {
    static constexpr char NEWLINE_CHAR = '\n';
    static constexpr char CARRIAGE_RETURN_CHAR = '\r';
    static constexpr char TAB_CHAR = '\t';
    static constexpr char SPACE_CHAR = ' ';
    static constexpr size_t FIRST_CHAR_INDEX = 0;
    static constexpr size_t SINGLE_CHAR_COUNT = 1;

    std::string result;
    result.reserve(text.size());

    bool previousWasSpace = false;

    for (char chr : text) {
        // Заменяем переносы строк и табуляции на пробелы
        if (chr == NEWLINE_CHAR || chr == CARRIAGE_RETURN_CHAR || chr == TAB_CHAR) {
            chr = SPACE_CHAR;
        }

        // Пропускаем множественные пробелы
        if (chr == SPACE_CHAR) {
            if (!previousWasSpace) {
                result += chr;
                previousWasSpace = true;
            }
        } else {
            result += chr;
            previousWasSpace = false;
        }
    }

    // Убираем пробел в начале, если есть
    if (!result.empty() && result[FIRST_CHAR_INDEX] == SPACE_CHAR) {
        result.erase(FIRST_CHAR_INDEX, SINGLE_CHAR_COUNT);
    }

    // Убираем пробел в конце, если есть
    if (!result.empty() && result[result.length() - SINGLE_CHAR_COUNT] == SPACE_CHAR) {
        result.erase(result.length() - SINGLE_CHAR_COUNT);
    }

    return result;
}

} // namespace Infrastructure::Text
