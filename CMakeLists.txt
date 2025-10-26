cmake_minimum_required(VERSION 3.16)

project(SearchSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

add_custom_target(CommonFiles SOURCES config.ini README.md)

add_subdirectory(Core)
add_subdirectory(Infrastructure)
add_subdirectory(SpiderData)
add_subdirectory(Spider)
add_subdirectory(HTTPServerData)
add_subdirectory(HTTPServer)
