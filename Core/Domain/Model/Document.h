#pragma once

#include <cstdint>
#include <string>

namespace Core::Domain::Model {
/**
 * @brief Сущность "Документ" - представляет веб-страницу в поисковой системе
 *
 * Документ содержит URL страницы и её текстовое содержимое.
 * ID документа генерируется базой данных при сохранении.
 */
class Document {
  public:
    using IdType = int64_t;

    /**
     * @brief Конструктор для нового документа (без ID)
     */
    Document(std::string url, std::string content);

    /**
     * @brief Конструктор для существующего документа (с ID из БД)
     */
    Document(IdType id, std::string url, std::string content);

    // Геттеры
    IdType getId() const;
    const std::string& getUrl() const;
    const std::string& getContent() const;

    // Проверка, сохранен ли документ в БД
    bool isPersisted() const;

    // Установка ID (используется репозиторием после сохранения)
    void setId(IdType id);

  private:
    IdType id_;  // 0 означает, что документ ещё не сохранен в БД
    std::string url_;
    std::string content_;
};
} // namespace Core::Domain::Model
