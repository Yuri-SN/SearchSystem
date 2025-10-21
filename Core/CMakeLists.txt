cmake_minimum_required(VERSION 3.16)

project(Core VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    Domain/ValueObject/Url.h
    Domain/ValueObject/Url.cpp
    Domain/ValueObject/SearchQuery.h
    Domain/ValueObject/SearchQuery.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
