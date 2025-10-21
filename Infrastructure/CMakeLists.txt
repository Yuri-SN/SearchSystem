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

    # TODO: Database
    # Database/DatabaseConnection.h
    # Database/DatabaseConnection.cpp
    # Database/PostgresDocumentRepository.h
    # Database/PostgresDocumentRepository.cpp
    # Database/PostgresWordRepository.h
    # Database/PostgresWordRepository.cpp

    # TODO: Http
    # Http/BoostBeastHttpClient.h
    # Http/BoostBeastHttpClient.cpp
    # Http/BoostBeastHttpServer.h
    # Http/BoostBeastHttpServer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Зависимость от Core
target_link_libraries(${PROJECT_NAME}
    PUBLIC Core
    PRIVATE Boost::locale
    PRIVATE Boost::system
    PRIVATE Boost::thread
    PRIVATE PkgConfig::PQXX
    PRIVATE PkgConfig::GUMBO
)

# Include директории
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
