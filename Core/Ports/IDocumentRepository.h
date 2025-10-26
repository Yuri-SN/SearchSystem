#pragma once

#include <optional>
#include <vector>

#include "../Domain/Model/Document.h"

namespace Core::Ports {
/**
 * @brief Интерфейс репозитория для работы с документами
 *
 * Порт для доступа к хранилищу документов.
 * Реализация будет в Infrastructure слое.
 */
class IDocumentRepository {
  public:
    virtual ~IDocumentRepository() = default;

    /**
     * @brief Сохраняет документ в хранилище
     * @param document Документ для сохранения
     * @return ID сохраненного документа
     */
    virtual Domain::Model::Document::IdType save(Domain::Model::Document& document) = 0;

    /**
     * @brief Находит документ по ID
     * @param id ID документа
     * @return Документ, если найден
     */
    virtual std::optional<Domain::Model::Document> findById(Domain::Model::Document::IdType id) = 0;

    /**
     * @brief Находит документ по URL
     * @param url URL документа
     * @return Документ, если найден
     */
    virtual std::optional<Domain::Model::Document> findByUrl(const std::string& url) = 0;

    /**
     * @brief Проверяет, существует ли документ с данным URL
     * @param url URL документа
     * @return true если документ существует
     */
    virtual bool existsByUrl(const std::string& url) = 0;

    /**
     * @brief Находит все документы
     * @return Список всех документов
     */
    virtual std::vector<Domain::Model::Document> findAll() = 0;
};
} // namespace Core::Ports
