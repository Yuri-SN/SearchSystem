#pragma once

#include <memory>
#include <string>

#include "../Core/Application/UseCases/IndexPageUseCase.h"
#include "../Core/Ports/IConfiguration.h"
#include "../Core/Ports/IDocumentRepository.h"
#include "../Core/Ports/IHtmlParser.h"
#include "../Core/Ports/IHttpClient.h"
#include "../Core/Ports/ITextProcessor.h"
#include "../Core/Ports/IWordRepository.h"

namespace SpiderData {
/**
 * @brief Контейнер зависимостей для приложения Spider
 *
 * Composition Root - место, где создаются и связываются все зависимости.
 * Реализует паттерн Dependency Injection для приложения-краулера.
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
     * @brief Получить Use Case для индексации страниц (singleton)
     * @return Shared pointer на IndexPageUseCase
     */
    std::shared_ptr<Core::Application::UseCases::IndexPageUseCase> getIndexPageUseCase();

    /**
     * @brief Создать новый Use Case для индексации страниц
     * Создаёт новый экземпляр с собственным подключением к БД.
     * Используется для многопоточной работы, где каждый поток должен иметь
     * своё собственное подключение к БД.
     * @return Shared pointer на новый IndexPageUseCase
     */
    std::shared_ptr<Core::Application::UseCases::IndexPageUseCase> createIndexPageUseCase();

    /**
     * @brief Получить конфигурацию
     * @return Shared pointer на IConfiguration
     */
    std::shared_ptr<Core::Ports::IConfiguration> getConfiguration();

  private:
    // Configuration
    std::shared_ptr<Core::Ports::IConfiguration> configuration_;

    // Infrastructure components
    std::shared_ptr<Core::Ports::IHttpClient> httpClient_;
    std::shared_ptr<Core::Ports::IHtmlParser> htmlParser_;
    std::shared_ptr<Core::Ports::ITextProcessor> textProcessor_;

    // Database
    std::shared_ptr<void> databaseConnection_;  // DatabaseConnection (void* чтобы
                                                // не включать заголовок)
    std::shared_ptr<Core::Ports::IDocumentRepository> documentRepository_;
    std::shared_ptr<Core::Ports::IWordRepository> wordRepository_;

    // Use Cases
    std::shared_ptr<Core::Application::UseCases::IndexPageUseCase> indexPageUseCase_;

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
} // namespace SpiderData
