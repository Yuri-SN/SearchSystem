cmake_minimum_required(VERSION 3.16)

project(Infrastructure VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    Adapters/PostgresRepository.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC Core
    PRIVATE Boost::locale
    PRIVATE Boost::system
    PRIVATE Boost::thread
    PRIVATE PkgConfig::PQXX
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
