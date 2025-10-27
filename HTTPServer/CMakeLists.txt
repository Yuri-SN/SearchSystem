cmake_minimum_required(VERSION 3.16)

project(HTTPServer VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE HTTPServerData)

configure_file(${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/cacert.pem ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem COPYONLY)
