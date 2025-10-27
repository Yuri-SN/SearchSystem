#include "DIContainer.h"

#include <sstream>

#include "../Infrastructure/Configuration/IniConfiguration.h"
#include "../Infrastructure/Database/DatabaseConnection.h"
#include "../Infrastructure/Database/PostgresDocumentRepository.h"
#include "../Infrastructure/Database/PostgresWordRepository.h"
#include "../Infrastructure/Http/BoostBeastHttpClient.h"
#include "../Infrastructure/Parsers/HtmlParser.h"
#include "../Infrastructure/Text/BoostLocaleTextProcessor.h"

namespace SpiderData {
DIContainer::DIContainer(const std::string& configPath) {
    // Загружаем конфигурацию
    configuration_ = std::make_shared<Infrastructure::Configuration::IniConfiguration>(configPath);

    // Инициализируем все зависимости
    initialize();
}

void DIContainer::initialize() {
    // 1. Создаём Infrastructure компоненты

    // HTTP клиент для скачивания страниц
    httpClient_ = std::make_shared<Infrastructure::Http::BoostBeastHttpClient>();

    // HTML парсер
    htmlParser_ = std::make_shared<Infrastructure::Parsers::HtmlParser>();

    // Текстовый процессор (используем русскую локаль)
    textProcessor_ =
        std::make_shared<Infrastructure::Text::BoostLocaleTextProcessor>("ru_RU.UTF-8");

    // 2. Создаём подключение к базе данных
    const std::string connectionString = createDatabaseConnectionString();
    auto dbConnection =
        std::make_shared<Infrastructure::Database::DatabaseConnection>(connectionString);

    // Создаём схему БД, если её нет
    dbConnection->createSchema();

    // Сохраняем указатель на соединение
    databaseConnection_ = dbConnection;

    // 3. Создаём репозитории
    documentRepository_ =
        std::make_shared<Infrastructure::Database::PostgresDocumentRepository>(dbConnection);
    wordRepository_ =
        std::make_shared<Infrastructure::Database::PostgresWordRepository>(dbConnection);

    // 4. Создаём Use Case для индексации
    // IndexPageUseCase создаёт IndexingService внутри себя
    indexPageUseCase_ = std::make_shared<Core::Application::UseCases::IndexPageUseCase>(
        documentRepository_, wordRepository_, htmlParser_, textProcessor_);
}

std::string DIContainer::createDatabaseConnectionString() const {
    const std::string host = configuration_->getDatabaseHost();
    const int port = configuration_->getDatabasePort();
    const std::string dbname = configuration_->getDatabaseName();
    const std::string user = configuration_->getDatabaseUser();
    const std::string password = configuration_->getDatabasePassword();

    std::ostringstream oss;
    oss << "host=" << host << " port=" << port << " dbname=" << dbname << " user=" << user
        << " password=" << password;

    return oss.str();
}

std::shared_ptr<Core::Application::UseCases::IndexPageUseCase> DIContainer::getIndexPageUseCase() {
    return indexPageUseCase_;
}

std::shared_ptr<Core::Application::UseCases::IndexPageUseCase>
DIContainer::createIndexPageUseCase() {
    // Создаём НОВОЕ подключение к базе данных для этого экземпляра
    const std::string connectionString = createDatabaseConnectionString();
    auto dbConnection =
        std::make_shared<Infrastructure::Database::DatabaseConnection>(connectionString);

    // Создаём новые репозитории с новым подключением
    auto documentRepository =
        std::make_shared<Infrastructure::Database::PostgresDocumentRepository>(dbConnection);
    auto wordRepository =
        std::make_shared<Infrastructure::Database::PostgresWordRepository>(dbConnection);

    // Создаём новый Use Case с новыми репозиториями
    // Используем общие (thread-safe) компоненты для парсинга
    return std::make_shared<Core::Application::UseCases::IndexPageUseCase>(
        documentRepository, wordRepository, htmlParser_, textProcessor_);
}

std::shared_ptr<Core::Ports::IConfiguration> DIContainer::getConfiguration() {
    return configuration_;
}
} // namespace SpiderData
