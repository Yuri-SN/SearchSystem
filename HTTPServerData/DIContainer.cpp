#include "DIContainer.h"

#include <sstream>

#include "../Infrastructure/Configuration/IniConfiguration.h"
#include "../Infrastructure/Database/DatabaseConnection.h"
#include "../Infrastructure/Database/PostgresWordRepository.h"
#include "../Infrastructure/Http/BoostBeastHttpServer.h"
#include "../Infrastructure/Text/BoostLocaleTextProcessor.h"

namespace HTTPServerData {
DIContainer::DIContainer(const std::string& configPath) {
    configuration_ = std::make_shared<Infrastructure::Configuration::IniConfiguration>(configPath);

    initialize();
}

void DIContainer::initialize() {
    textProcessor_ =
        std::make_shared<Infrastructure::Text::BoostLocaleTextProcessor>("ru_RU.UTF-8");

    httpServer_ = std::make_shared<Infrastructure::Http::BoostBeastHttpServer>(4);

    const std::string connectionString = createDatabaseConnectionString();
    auto dbConnection =
        std::make_shared<Infrastructure::Database::DatabaseConnection>(connectionString);

    dbConnection->createSchema();

    databaseConnection_ = dbConnection;

    wordRepository_ =
        std::make_shared<Infrastructure::Database::PostgresWordRepository>(dbConnection);

    searchDocumentsUseCase_ = std::make_shared<Core::Application::UseCases::SearchDocumentsUseCase>(
        wordRepository_, textProcessor_);
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

std::shared_ptr<Core::Application::UseCases::SearchDocumentsUseCase>
DIContainer::getSearchDocumentsUseCase() {
    return searchDocumentsUseCase_;
}

std::shared_ptr<Core::Ports::IHttpServer> DIContainer::getHttpServer() {
    return httpServer_;
}

std::shared_ptr<Core::Ports::IConfiguration> DIContainer::getConfiguration() {
    return configuration_;
}
} // namespace HTTPServerData
