#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Core::Domain::ValueObject {
/**
 * @brief Value Object поискового запроса
 *
 * Представляет поисковый запрос, разбитый на отдельные слова (термы).
 * Неизменяемый объект. Ограничение: не более 4 слов.
 */
class SearchQuery {
  public:
    static constexpr size_t MAX_TERMS = 4;

    /**
     * @brief Создает поисковый запрос из строки
     * @param query Строка запроса
     * @return Объект SearchQuery если валидация успешна, иначе std::nullopt
     */
    static std::optional<SearchQuery> create(const std::string& query);

    /**
     * @brief Получить термы (слова) запроса
     */
    const std::vector<std::string>& getTerms() const;

    /**
     * @brief Получить оригинальную строку запроса
     */
    const std::string& toString() const;

    /**
     * @brief Проверка, пуст ли запрос
     */
    bool isEmpty() const;

  private:
    /**
     * @brief Приватный конструктор (используется фабричным методом create)
     */
    SearchQuery(std::string query, std::vector<std::string> terms);

    /**
     * @brief Парсинг и валидация запроса
     */
    static bool parse(const std::string& query, std::vector<std::string>& terms);

    std::string query_;
    std::vector<std::string> terms_;
};
} // namespace Core::Domain::ValueObject
