#pragma once

#include <string>

namespace Core::Ports {
/**
 * @brief Интерфейс для обработки текста
 *
 * Порт для операций обработки текста (приведение к нижнему регистру и т.д.).
 * Реализация будет в Infrastructure слое (Boost Locale).
 */
class ITextProcessor {
  public:
    virtual ~ITextProcessor() = default;

    /**
     * @brief Приводит текст к нижнему регистру с учетом локали
     * @param text Исходный текст
     * @return Текст в нижнем регистре
     */
    virtual std::string toLowercase(const std::string& text) = 0;

    /**
     * @brief Нормализует текст (убирает лишние пробелы, переносы строк)
     * @param text Исходный текст
     * @return Нормализованный текст
     */
    virtual std::string normalize(const std::string& text) = 0;
};
} // namespace Core::Ports
