cmake_minimum_required(VERSION 3.16)

project(Infrastructure VERSION 0.1 LANGUAGES CXX)

# add_library(${PROJECT_NAME} STATIC ${SOURCES})

# target_link_libraries(${PROJECT_NAME} PRIVATE Core)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
