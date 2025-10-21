#include "DatabaseConnection.h"

#include <stdexcept>

namespace Infrastructure::Database {

DatabaseConnection::DatabaseConnection(const std::string& connectionString) : connectionString_(connectionString) {
    try {
        connection_ = std::make_unique<pqxx::connection>(connectionString_);
    } catch (const std::exception& e) {
        throw std::runtime_error("Не удалось подключиться к базе данных: " + std::string(e.what()));
    }
}

DatabaseConnection::~DatabaseConnection() = default;

pqxx::connection& DatabaseConnection::getConnection() {
    if (!connection_) {
        throw std::runtime_error("Соединение с базой данных не установлено");
    }
    return *connection_;
}

bool DatabaseConnection::isConnected() const {
    return connection_ && connection_->is_open();
}

void DatabaseConnection::createSchema() {
    if (!isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(*connection_);

        // Создаём таблицы
        createDocumentsTable(txn);
        createWordsTable(txn);
        createWordFrequenciesTable(txn);

        // Создаём индексы
        createIndexes(txn);

        // Фиксируем транзакцию
        txn.commit();
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при создании схемы БД: " + std::string(e.what()));
    }
}

void DatabaseConnection::createDocumentsTable(pqxx::work& txn) {
    static constexpr int MAX_URL_LENGTH = 2048;

    const std::string sql = R"(
        CREATE TABLE IF NOT EXISTS documents (
            id BIGSERIAL PRIMARY KEY,
            url VARCHAR()" + std::to_string(MAX_URL_LENGTH) +
                            R"() UNIQUE NOT NULL,
            content TEXT NOT NULL
        )
    )";

    txn.exec(sql);
}

void DatabaseConnection::createWordsTable(pqxx::work& txn) {
    static constexpr int MAX_WORD_LENGTH = 32;

    const std::string sql = R"(
        CREATE TABLE IF NOT EXISTS words (
            id BIGSERIAL PRIMARY KEY,
            text VARCHAR()" +
                            std::to_string(MAX_WORD_LENGTH) + R"() UNIQUE NOT NULL
        )
    )";

    txn.exec(sql);
}

void DatabaseConnection::createWordFrequenciesTable(pqxx::work& txn) {
    const std::string sql = R"(
        CREATE TABLE IF NOT EXISTS word_frequencies (
            document_id BIGINT REFERENCES documents(id) ON DELETE CASCADE,
            word_id BIGINT REFERENCES words(id) ON DELETE CASCADE,
            frequency INTEGER NOT NULL,
            PRIMARY KEY (document_id, word_id)
        )
    )";

    txn.exec(sql);
}

void DatabaseConnection::createIndexes(pqxx::work& txn) {
    // Индекс на url для быстрого поиска документов по URL
    txn.exec(R"(
        CREATE INDEX IF NOT EXISTS idx_documents_url
        ON documents(url)
    )");

    // Индекс на text для быстрого поиска слов
    txn.exec(R"(
        CREATE INDEX IF NOT EXISTS idx_words_text
        ON words(text)
    )");

    // Индекс на word_id для быстрого поиска по словам
    txn.exec(R"(
        CREATE INDEX IF NOT EXISTS idx_word_frequencies_word_id
        ON word_frequencies(word_id)
    )");

    // Индекс на document_id для быстрого поиска по документам
    txn.exec(R"(
        CREATE INDEX IF NOT EXISTS idx_word_frequencies_document_id
        ON word_frequencies(document_id)
    )");
}

} // namespace Infrastructure::Database
