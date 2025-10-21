#pragma once

#include <boost/locale.hpp>
#include <string>

#include "../../Core/Ports/ITextProcessor.h"

namespace Infrastructure::Text {

/**
 * @brief Реализация ITextProcessor с использованием Boost.Locale
 *
 * Использует Boost.Locale для корректной обработки текста с учётом локали.
 * Поддерживает работу с UTF-8 и различными языками.
 */
class BoostLocaleTextProcessor : public Core::Ports::ITextProcessor {
  public:
    /**
     * @brief Конструктор
     * @param localeName Имя локали (например, "ru_RU.UTF-8", "en_US.UTF-8",
     * "tr_TR.UTF-8")
     */
    explicit BoostLocaleTextProcessor(const std::string& localeName = "ru_RU.UTF-8");

    ~BoostLocaleTextProcessor() override = default;

    /**
     * @brief Приводит текст к нижнему регистру с учетом локали
     * @param text Исходный текст
     * @return Текст в нижнем регистре
     */
    std::string toLowercase(const std::string& text) override;

    /**
     * @brief Нормализует текст (убирает лишние пробелы, переносы строк)
     * @param text Исходный текст
     * @return Нормализованный текст
     */
    std::string normalize(const std::string& text) override;

  private:
    boost::locale::generator gen_;  // Генератор локалей
    std::locale locale_;            // Текущая локаль
};

} // namespace Infrastructure::Text
