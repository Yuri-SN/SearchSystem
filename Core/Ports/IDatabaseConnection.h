#pragma once

namespace pqxx {
class connection;
}

namespace Core::Ports {
/**
 * @brief Интерфейс для управления соединением с базой данных
 *
 * Абстракция для подключения к БД, позволяющая скрыть конкретную
 * реализацию (PostgreSQL) от слоя приложения.
 *
 * Следует Dependency Inversion Principle - высокоуровневые модули
 * зависят от абстракции, а не от конкретной реализации.
 */
class IDatabaseConnection {
  public:
    virtual ~IDatabaseConnection() = default;

    /**
     * @brief Получить соединение с БД
     * @return Ссылка на объект соединения
     */
    virtual pqxx::connection& getConnection() = 0;

    /**
     * @brief Создаёт схему базы данных (таблицы и индексы)
     *
     * Создаёт необходимые таблицы если их нет.
     * Идемпотентная операция - можно вызывать многократно.
     */
    virtual void createSchema() = 0;

    /**
     * @brief Проверяет, установлено ли соединение с БД
     * @return true если соединение активно
     */
    virtual bool isConnected() const = 0;
};
} // namespace Core::Ports
