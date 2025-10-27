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
    configuration_ = std::make_shared<Infrastructure::Configuration::IniConfiguration>(configPath);

    initialize();
}

void DIContainer::initialize() {
    httpClient_ = std::make_shared<Infrastructure::Http::BoostBeastHttpClient>();

    htmlParser_ = std::make_shared<Infrastructure::Parsers::HtmlParser>();

    textProcessor_ =
        std::make_shared<Infrastructure::Text::BoostLocaleTextProcessor>("ru_RU.UTF-8");

    const std::string connectionString = createDatabaseConnectionString();
    auto dbConnection =
        std::make_shared<Infrastructure::Database::DatabaseConnection>(connectionString);

    dbConnection->createSchema();

    databaseConnection_ = dbConnection;

    documentRepository_ =
        std::make_shared<Infrastructure::Database::PostgresDocumentRepository>(dbConnection);
    wordRepository_ =
        std::make_shared<Infrastructure::Database::PostgresWordRepository>(dbConnection);

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
