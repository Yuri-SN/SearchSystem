cmake_minimum_required(VERSION 3.16)

project(Spider VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SpiderData)
