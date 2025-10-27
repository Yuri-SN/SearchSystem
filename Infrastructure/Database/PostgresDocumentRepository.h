#pragma once

#include <memory>

#include "../../Core/Ports/IDocumentRepository.h"
#include "DatabaseConnection.h"

namespace Infrastructure::Database {
/**
 * @brief PostgreSQL реализация репозитория документов
 *
 * Использует libpqxx для работы с PostgreSQL базой данных.
 * Реализует все операции CRUD для документов.
 */
class PostgresDocumentRepository : public Core::Ports::IDocumentRepository {
  public:
    /**
     * @brief Конструктор
     * @param dbConnection Соединение с базой данных
     */
    explicit PostgresDocumentRepository(std::shared_ptr<DatabaseConnection> dbConnection);

    ~PostgresDocumentRepository() override = default;

    /**
     * @brief Сохраняет документ в базе данных
     * @param document Документ для сохранения
     * @return ID сохраненного документа
     *
     * Если документ с таким URL уже существует, обновляет его содержимое.
     * Если документ новый, вставляет новую запись.
     */
    Core::Domain::Model::Document::IdType save(Core::Domain::Model::Document& document) override;

    /**
     * @brief Находит документ по ID
     * @param id ID документа
     * @return Документ, если найден
     */
    std::optional<Core::Domain::Model::Document> findById(
        Core::Domain::Model::Document::IdType id) override;

    /**
     * @brief Находит документ по URL
     * @param url URL документа
     * @return Документ, если найден
     */
    std::optional<Core::Domain::Model::Document> findByUrl(const std::string& url) override;

    /**
     * @brief Проверяет, существует ли документ с данным URL
     * @param url URL документа
     * @return true если документ существует
     */
    bool existsByUrl(const std::string& url) override;

    /**
     * @brief Находит все документы
     * @return Список всех документов
     */
    std::vector<Core::Domain::Model::Document> findAll() override;

  private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
};
} // namespace Infrastructure::Database
