cmake_minimum_required(VERSION 3.16)

project(Infrastructure VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    # Configuration
    Configuration/IniConfiguration.h
    Configuration/IniConfiguration.cpp

    # Text
    Text/BoostLocaleTextProcessor.h
    Text/BoostLocaleTextProcessor.cpp

    # Parsers
    Parsers/HtmlParser.h
    Parsers/HtmlParser.cpp

    # Database
    Database/DatabaseConnection.h
    Database/DatabaseConnection.cpp
    Database/PostgresDocumentRepository.h
    Database/PostgresDocumentRepository.cpp
    Database/PostgresWordRepository.h
    Database/PostgresWordRepository.cpp

    # Http
    Http/BoostBeastHttpClient.h
    Http/BoostBeastHttpClient.cpp
    Http/BoostBeastHttpServer.h
    Http/BoostBeastHttpServer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC Core
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::locale
    PRIVATE Boost::system
    PRIVATE Boost::thread
    PRIVATE OpenSSL::SSL
    PRIVATE OpenSSL::Crypto
    #PRIVATE PkgConfig::PQXX
    #PRIVATE PkgConfig::GUMBO
)

target_link_libraries(${PROJECT_NAME} PUBLIC libpqxx::pqxx)
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::gumbo::gumbo)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
