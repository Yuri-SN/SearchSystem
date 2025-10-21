#pragma once

#include <memory>
#include <pqxx/pqxx>
#include <string>

namespace Infrastructure::Database {

/**
 * @brief Утилитный класс для управления соединением с PostgreSQL
 *
 * Управляет подключением к базе данных, создаёт схему таблиц,
 * предоставляет доступ к connection для выполнения запросов.
 */
class DatabaseConnection {
  public:
    /**
     * @brief Конструктор
     * @param connectionString Строка подключения к PostgreSQL
     *        Формат: "host=localhost port=5432 dbname=search_system user=postgres
     * password=secret"
     */
    explicit DatabaseConnection(const std::string& connectionString);

    /**
     * @brief Деструктор
     */
    ~DatabaseConnection();

    // Запрещаем копирование
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    // Разрешаем перемещение
    DatabaseConnection(DatabaseConnection&&) noexcept = default;
    DatabaseConnection& operator=(DatabaseConnection&&) noexcept = default;

    /**
     * @brief Получить соединение с БД
     * @return Ссылка на объект соединения
     */
    pqxx::connection& getConnection();

    /**
     * @brief Создаёт схему базы данных (таблицы и индексы)
     *
     * Создаёт таблицы documents, words, word_frequencies если их нет.
     * Идемпотентная операция - можно вызывать многократно.
     */
    void createSchema();

    /**
     * @brief Проверяет, установлено ли соединение с БД
     * @return true если соединение активно
     */
    bool isConnected() const;

  private:
    /**
     * @brief Выполняет SQL-запрос для создания таблицы documents
     */
    static void createDocumentsTable(pqxx::work& txn);

    /**
     * @brief Выполняет SQL-запрос для создания таблицы words
     */
    static void createWordsTable(pqxx::work& txn);

    /**
     * @brief Выполняет SQL-запрос для создания таблицы word_frequencies
     */
    static void createWordFrequenciesTable(pqxx::work& txn);

    /**
     * @brief Создаёт индексы для ускорения поиска
     */
    static void createIndexes(pqxx::work& txn);

    std::unique_ptr<pqxx::connection> connection_;
    std::string connectionString_;
};

} // namespace Infrastructure::Database
