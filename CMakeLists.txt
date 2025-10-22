cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0167 OLD)

set(QT_CREATOR_SKIP_MAINTENANCE_TOOL_PROVIDER ON)

project(SearchSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS locale system thread)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)
pkg_check_modules(GUMBO REQUIRED IMPORTED_TARGET gumbo)

include(GNUInstallDirs)

add_custom_target(CommonFiles SOURCES config.ini README.md)

add_subdirectory(Core)
add_subdirectory(Infrastructure)
add_subdirectory(SpiderData)
add_subdirectory(Spider)
add_subdirectory(HTTPServerData)
add_subdirectory(HTTPServer)
