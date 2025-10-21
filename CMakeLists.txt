cmake_minimum_required(VERSION 3.16)

project(SearchSystem VERSION 0.1 LANGUAGES CXX)

set(QT_CREATOR_SKIP_MAINTENANCE_TOOL_PROVIDER ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

add_subdirectory(Core)
# add_subdirectory(Infrastructure)
# add_subdirectory(Spider)
# add_subdirectory(SpiderData)
# add_subdirectory(HTTPServer)
# add_subdirectory(HTTPServerData)

# install(TARGETS SearchSystem
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )
