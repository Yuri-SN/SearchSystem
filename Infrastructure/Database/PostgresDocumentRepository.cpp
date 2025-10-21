#include "PostgresDocumentRepository.h"

#include <stdexcept>

namespace Infrastructure::Database {

PostgresDocumentRepository::PostgresDocumentRepository(std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {
    if (!dbConnection_) {
        throw std::invalid_argument("DatabaseConnection не может быть nullptr");
    }
}

Core::Domain::Model::Document::IdType PostgresDocumentRepository::save(Core::Domain::Model::Document& document) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        // Проверяем, существует ли документ с таким URL
        const std::string checkSql = "SELECT id FROM documents WHERE url = $1";
        pqxx::result checkResult = txn.exec(checkSql, pqxx::params(document.getUrl()));

        Core::Domain::Model::Document::IdType documentId = 0;

        if (!checkResult.empty()) {
            // Документ существует - обновляем его содержимое
            documentId = checkResult[0][0].as<Core::Domain::Model::Document::IdType>();

            const std::string updateSql = "UPDATE documents SET content = $1 WHERE id = $2";
            txn.exec(updateSql, pqxx::params(document.getContent(), documentId));
        } else {
            // Документ не существует - вставляем новый
            const std::string insertSql = "INSERT INTO documents (url, content) VALUES ($1, $2) RETURNING id";
            pqxx::result insertResult =
                txn.exec(insertSql, pqxx::params(document.getUrl(), document.getContent()));

            documentId = insertResult[0][0].as<Core::Domain::Model::Document::IdType>();
        }

        txn.commit();

        // Обновляем ID документа
        document.setId(documentId);

        return documentId;
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при сохранении документа: " + std::string(e.what()));
    }
}

std::optional<Core::Domain::Model::Document> PostgresDocumentRepository::findById(
    Core::Domain::Model::Document::IdType id) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        const std::string sql = "SELECT id, url, content FROM documents WHERE id = $1";
        pqxx::result result = txn.exec(sql, pqxx::params(id));

        if (result.empty()) {
            return std::nullopt;
        }

        const auto& row = result[0];
        return Core::Domain::Model::Document(row[0].as<Core::Domain::Model::Document::IdType>(),
                                             row[1].as<std::string>(), row[2].as<std::string>());
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при поиске документа по ID: " + std::string(e.what()));
    }
}

std::optional<Core::Domain::Model::Document> PostgresDocumentRepository::findByUrl(const std::string& url) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        const std::string sql = "SELECT id, url, content FROM documents WHERE url = $1";
        pqxx::result result = txn.exec(sql, pqxx::params(url));

        if (result.empty()) {
            return std::nullopt;
        }

        const auto& row = result[0];
        return Core::Domain::Model::Document(row[0].as<Core::Domain::Model::Document::IdType>(),
                                             row[1].as<std::string>(), row[2].as<std::string>());
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при поиске документа по URL: " + std::string(e.what()));
    }
}

bool PostgresDocumentRepository::existsByUrl(const std::string& url) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        const std::string sql = "SELECT EXISTS(SELECT 1 FROM documents WHERE url = $1)";
        pqxx::result result = txn.exec(sql, pqxx::params(url));

        return result[0][0].as<bool>();
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при проверке существования документа: " + std::string(e.what()));
    }
}

std::vector<Core::Domain::Model::Document> PostgresDocumentRepository::findAll() {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        const std::string sql = "SELECT id, url, content FROM documents ORDER BY id";
        pqxx::result result = txn.exec(sql);

        std::vector<Core::Domain::Model::Document> documents;
        documents.reserve(result.size());

        for (const auto& row : result) {
            documents.emplace_back(row[0].as<Core::Domain::Model::Document::IdType>(), row[1].as<std::string>(),
                                   row[2].as<std::string>());
        }

        return documents;
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при получении всех документов: " + std::string(e.what()));
    }
}

} // namespace Infrastructure::Database
