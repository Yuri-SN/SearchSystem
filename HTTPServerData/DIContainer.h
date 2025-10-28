#pragma once

#include <memory>
#include <string>

#include "../Core/Application/UseCases/SearchDocumentsUseCase.h"
#include "../Core/Ports/IConfiguration.h"
#include "../Core/Ports/IDatabaseConnection.h"
#include "../Core/Ports/IHttpServer.h"
#include "../Core/Ports/ITextProcessor.h"
#include "../Core/Ports/IWordRepository.h"

namespace HTTPServerData {
/**
 * @brief Контейнер зависимостей для приложения HTTPServer
 *
 * Composition Root - место, где создаются и связываются все зависимости.
 * Реализует паттерн Dependency Injection для поискового HTTP-сервера.
 */
class DIContainer {
  public:
    /**
     * @brief Конструктор
     * @param configPath Путь к файлу конфигурации (config.ini)
     */
    explicit DIContainer(const std::string& configPath);

    ~DIContainer() = default;

    // Запрещаем копирование и перемещение
    DIContainer(const DIContainer&) = delete;
    DIContainer& operator=(const DIContainer&) = delete;
    DIContainer(DIContainer&&) = delete;
    DIContainer& operator=(DIContainer&&) = delete;

    /**
     * @brief Получить Use Case для поиска документов
     * @return Shared pointer на SearchDocumentsUseCase
     */
    std::shared_ptr<Core::Application::UseCases::SearchDocumentsUseCase>
    getSearchDocumentsUseCase();

    /**
     * @brief Получить HTTP-сервер
     * @return Shared pointer на IHttpServer
     */
    std::shared_ptr<Core::Ports::IHttpServer> getHttpServer();

    /**
     * @brief Получить конфигурацию
     * @return Shared pointer на IConfiguration
     */
    std::shared_ptr<Core::Ports::IConfiguration> getConfiguration();

  private:
    // Configuration
    std::shared_ptr<Core::Ports::IConfiguration> configuration_;

    // Infrastructure components
    std::shared_ptr<Core::Ports::ITextProcessor> textProcessor_;
    std::shared_ptr<Core::Ports::IHttpServer> httpServer_;

    // Database
    std::shared_ptr<Core::Ports::IDatabaseConnection> databaseConnection_;
    std::shared_ptr<Core::Ports::IWordRepository> wordRepository_;

    // Use Cases
    std::shared_ptr<Core::Application::UseCases::SearchDocumentsUseCase> searchDocumentsUseCase_;

    /**
     * @brief Инициализирует все зависимости
     */
    void initialize();

    /**
     * @brief Создаёт строку подключения к базе данных из конфигурации
     * @return Строка подключения PostgreSQL
     */
    std::string createDatabaseConnectionString() const;
};
} // namespace HTTPServerData
