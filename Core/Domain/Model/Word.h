#pragma once

#include <cstdint>
#include <string>

namespace Core::Domain::Model {
/**
 * @brief Сущность "Слово" - представляет уникальное слово в поисковой системе
 *
 * Слово хранится в нижнем регистре для обеспечения регистронезависимого поиска.
 */
class Word {
  public:
    using IdType = int64_t;

    /**
     * @brief Конструктор для нового слова (без ID)
     */
    explicit Word(std::string text);

    /**
     * @brief Конструктор для существующего слова (с ID из БД)
     */
    Word(IdType id, std::string text);

    // Геттеры
    IdType getId() const;
    const std::string& getText() const;

    // Проверка, сохранено ли слово в БД
    bool isPersisted() const;

    // Установка ID (используется репозиторием после сохранения)
    void setId(IdType id);

  private:
    IdType id_;         // 0 означает, что слово ещё не сохранено в БД
    std::string text_;  // Слово в нижнем регистре
};
} // namespace Core::Domain::Model
