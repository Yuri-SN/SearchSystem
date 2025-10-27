#include "PostgresWordRepository.h"

#include <sstream>
#include <stdexcept>

namespace Infrastructure::Database {
PostgresWordRepository::PostgresWordRepository(std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {
    if (!dbConnection_) {
        throw std::invalid_argument("DatabaseConnection не может быть nullptr");
    }
}

Core::Domain::Model::Word::IdType PostgresWordRepository::save(Core::Domain::Model::Word& word) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        // Используем INSERT ... ON CONFLICT для атомарной операции
        const std::string sql = R"(
            INSERT INTO words (text)
            VALUES ($1)
            ON CONFLICT (text) DO UPDATE SET text = EXCLUDED.text
            RETURNING id
        )";

        pqxx::result result = txn.exec(sql, pqxx::params(word.getText()));
        const auto wordId = result[0][0].as<Core::Domain::Model::Word::IdType>();

        txn.commit();

        // Обновляем ID слова
        word.setId(wordId);

        return wordId;
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при сохранении слова: " + std::string(e.what()));
    }
}

std::optional<Core::Domain::Model::Word> PostgresWordRepository::findByText(
    const std::string& text) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        const std::string sql = "SELECT id, text FROM words WHERE text = $1";
        pqxx::result result = txn.exec(sql, pqxx::params(text));

        if (result.empty()) {
            return std::nullopt;
        }

        const auto& row = result[0];
        return Core::Domain::Model::Word(row[0].as<Core::Domain::Model::Word::IdType>(),
                                         row[1].as<std::string>());
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при поиске слова: " + std::string(e.what()));
    }
}

void PostgresWordRepository::saveFrequency(const Core::Domain::Model::WordFrequency& frequency) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        // Используем INSERT ... ON CONFLICT для обновления существующей записи
        const std::string sql = R"(
            INSERT INTO word_frequencies (document_id, word_id, frequency)
            VALUES ($1, $2, $3)
            ON CONFLICT (document_id, word_id) DO UPDATE
            SET frequency = EXCLUDED.frequency
        )";

        txn.exec(sql, pqxx::params(frequency.getDocumentId(), frequency.getWordId(),
                                   frequency.getFrequency()));
        txn.commit();
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при сохранении частотности: " + std::string(e.what()));
    }
}

void PostgresWordRepository::saveWordFrequencies(
    Core::Domain::Model::Document::IdType documentId,
    const std::map<std::string, int>& wordFrequencies) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    if (wordFrequencies.empty()) {
        return;
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        // Шаг 1: Создаём все слова (если не существуют) и получаем их ID
        std::map<std::string, Core::Domain::Model::Word::IdType> wordIds;

        for (const auto& [wordText, frequency] : wordFrequencies) {
            const std::string wordSql = R"(
                INSERT INTO words (text)
                VALUES ($1)
                ON CONFLICT (text) DO UPDATE SET text = EXCLUDED.text
                RETURNING id
            )";

            pqxx::result wordResult = txn.exec(wordSql, pqxx::params(wordText));
            wordIds[wordText] = wordResult[0][0].as<Core::Domain::Model::Word::IdType>();
        }

        // Шаг 2: Пакетная вставка частотностей
        // Строим один большой INSERT для всех записей
        std::ostringstream sql;
        sql << R"(
            INSERT INTO word_frequencies (document_id, word_id, frequency)
            VALUES
        )";

        bool first = true;
        std::vector<pqxx::params> paramsList;
        int paramIndex = 1;
        std::vector<int64_t> allParams;

        for (const auto& [wordText, frequency] : wordFrequencies) {
            if (!first) {
                sql << ", ";
            }
            first = false;

            const int64_t wordId = wordIds[wordText];
            sql << "($" << paramIndex++ << ", $" << paramIndex++ << ", $" << paramIndex++ << ")";

            allParams.push_back(documentId);
            allParams.push_back(wordId);
            allParams.push_back(frequency);
        }

        sql << R"(
            ON CONFLICT (document_id, word_id) DO UPDATE
            SET frequency = EXCLUDED.frequency
        )";

        // Выполняем пакетную вставку
        if (!allParams.empty()) {
            // Создаём params с правильным количеством параметров
            pqxx::params params;
            for (const auto& param : allParams) {
                params.append(param);
            }

            txn.exec(sql.str(), params);
        }

        txn.commit();
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при сохранении частотностей слов: " +
                                 std::string(e.what()));
    }
}

std::vector<Core::Domain::Model::SearchResult> PostgresWordRepository::search(
    const std::vector<std::string>& words) {
    if (!dbConnection_->isConnected()) {
        throw std::runtime_error("Нет соединения с базой данных");
    }

    if (words.empty()) {
        return {};
    }

    try {
        pqxx::work txn(dbConnection_->getConnection());

        // Сложный SQL-запрос для поиска документов, содержащих ВСЕ указанные слова
        // Используем GROUP BY и HAVING для проверки, что документ содержит все
        // слова
        std::ostringstream sql;
        sql << R"(
            SELECT
                d.id AS document_id,
                d.url,
                SUM(wf.frequency) AS relevance
            FROM documents d
            INNER JOIN word_frequencies wf ON d.id = wf.document_id
            INNER JOIN words w ON wf.word_id = w.id
            WHERE w.text IN (
        )";

        // Добавляем плейсхолдеры для слов
        for (size_t i = 0; i < words.size(); ++i) {
            if (i > 0) {
                sql << ", ";
            }
            sql << "$" << (i + 1);
        }

        sql << R"(
            )
            GROUP BY d.id, d.url
            HAVING COUNT(DISTINCT w.id) = $)"
            << (words.size() + 1) << R"(
            ORDER BY relevance DESC
        )";

        // Создаём параметры
        pqxx::params params;
        for (const auto& word : words) {
            params.append(word);
        }
        params.append(static_cast<int64_t>(words.size()));

        pqxx::result result = txn.exec(sql.str(), params);

        // Преобразуем результаты в SearchResult
        std::vector<Core::Domain::Model::SearchResult> results;
        results.reserve(result.size());

        for (const auto& row : result) {
            results.emplace_back(row[0].as<Core::Domain::Model::SearchResult::DocumentIdType>(),
                                 row[1].as<std::string>(),
                                 row[2].as<Core::Domain::Model::SearchResult::RelevanceType>());
        }

        return results;
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при поиске документов: " + std::string(e.what()));
    }
}

Core::Domain::Model::Word::IdType PostgresWordRepository::getOrCreateWordId(
    const std::string& text) {
    const std::string sql = R"(
        INSERT INTO words (text)
        VALUES ($1)
        ON CONFLICT (text) DO UPDATE SET text = EXCLUDED.text
        RETURNING id
    )";

    pqxx::work txn(dbConnection_->getConnection());
    pqxx::result result = txn.exec(sql, pqxx::params(text));
    txn.commit();

    return result[0][0].as<Core::Domain::Model::Word::IdType>();
}
} // namespace Infrastructure::Database
