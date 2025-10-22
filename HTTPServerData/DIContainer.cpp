#include "DIContainer.h"

#include <sstream>

#include "../Infrastructure/Configuration/IniConfiguration.h"
#include "../Infrastructure/Database/DatabaseConnection.h"
#include "../Infrastructure/Database/PostgresWordRepository.h"
#include "../Infrastructure/Http/BoostBeastHttpServer.h"
#include "../Infrastructure/Text/BoostLocaleTextProcessor.h"

namespace HTTPServerData {

DIContainer::DIContainer(const std::string& configPath) {
    // 03@C605< :>=D83C@0F8N
    configuration_ = std::make_shared<Infrastructure::Configuration::IniConfiguration>(configPath);

    // =8F80;878@C5< 2A5 7028A8<>AB8
    initialize();
}

void DIContainer::initialize() {
    // 1. !>740Q< Infrastructure :><?>=5=BK

    // "5:AB>2K9 ?@>F5AA>@ (8A?>;L7C5< @CAA:CN ;>:0;L)
    textProcessor_ = std::make_shared<Infrastructure::Text::BoostLocaleTextProcessor>("ru_RU.UTF-8");

    // HTTP A5@25@ (:>;8G5AB2> ?>B>:>2 <>6=> 1K;> 1K 2K=5AB8 2 :>=D83C@0F8N)
    httpServer_ = std::make_shared<Infrastructure::Http::BoostBeastHttpServer>(4);

    // 2. !>740Q< ?>4:;NG5=85 : 1075 40==KE
    const std::string connectionString = createDatabaseConnectionString();
    auto dbConnection = std::make_shared<Infrastructure::Database::DatabaseConnection>(connectionString);

    // !>740Q< AE5<C , 5A;8 5Q =5B (=0 A;CG09 ?5@2>3> 70?CA:0)
    dbConnection->createSchema();

    // !>E@0=O5< C:070B5;L =0 A>548=5=85
    databaseConnection_ = dbConnection;

    // 3. !>740Q< @5?>78B>@89 A;>2 (4;O ?>8A:0 =C65= B>;L:> >=)
    wordRepository_ = std::make_shared<Infrastructure::Database::PostgresWordRepository>(dbConnection);

    // 4. !>740Q< Use Case 4;O ?>8A:0
    searchDocumentsUseCase_ =
        std::make_shared<Core::Application::UseCases::SearchDocumentsUseCase>(wordRepository_, textProcessor_);
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

std::shared_ptr<Core::Application::UseCases::SearchDocumentsUseCase> DIContainer::getSearchDocumentsUseCase() {
    return searchDocumentsUseCase_;
}

std::shared_ptr<Core::Ports::IHttpServer> DIContainer::getHttpServer() {
    return httpServer_;
}

std::shared_ptr<Core::Ports::IConfiguration> DIContainer::getConfiguration() {
    return configuration_;
}

} // namespace HTTPServerData
