cmake_minimum_required(VERSION 3.16)

project(Core VERSION 0.1 LANGUAGES CXX)

set(SOURCES
    Domain/ValueObject/Url.h
    Domain/ValueObject/Url.cpp
    Domain/ValueObject/SearchQuery.h
    Domain/ValueObject/SearchQuery.cpp

    Domain/Model/Document.h
    Domain/Model/Document.cpp
    Domain/Model/SearchResult.h
    Domain/Model/SearchResult.cpp
    Domain/Model/Word.h
    Domain/Model/Word.cpp
    Domain/Model/WordFrequency.h
    Domain/Model/WordFrequency.cpp

    DTO/CrawlResultDTO.h
    DTO/SearchRequestDTO.h
    DTO/SearchResponseDTO.h

    Ports/IConfiguration.h
    Ports/IDocumentRepository.h
    Ports/IHtmlParser.h
    Ports/IHttpClient.h
    Ports/IHttpServer.h
    Ports/ITextProcessor.h
    Ports/IWordRepository.h

    Domain/Service/IndexingService.h
    Domain/Service/IndexingService.cpp
    Domain/Service/RankingService.h
    Domain/Service/RankingService.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
