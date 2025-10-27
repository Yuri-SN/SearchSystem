cmake_minimum_required(VERSION 3.16)

project(HTTPServerData VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    DIContainer.h
    DIContainer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC Core
    PUBLIC Infrastructure
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
