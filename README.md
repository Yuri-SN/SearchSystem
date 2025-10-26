# Дипломный проект «Поисковая система»

## Описание проекта

Поисковая система состоит из двух приложений:

1. **Spider** - программа-краулер для индексации веб-страниц
2. **HTTPServer** - HTTP-сервер для обработки поисковых запросов

## Архитектура

Проект построен по принципам **Чистой Архитектуры** с разделением на слои:

### Слои архитектуры (от внешних к внутренним)

**Layer 1: Приложения (Entry Points)**
- `Spider` (main.cpp) - программа-краулер
- `HTTPServer` (main.cpp) - HTTP-сервер для поиска

↓ *зависят от*

**Layer 2: DI Containers (Composition Root)**
- `SpiderData` - контейнер зависимостей для Spider
- `HTTPServerData` - контейнер зависимостей для HTTPServer

↓ *собирают и связывают*

**Layer 3: Infrastructure Layer (Адаптеры)**

*Реализации интерфейсов для внешних систем:*
- `PostgresDocumentRepository` - работа с документами в БД
- `PostgresWordRepository` - работа со словами в БД
- `BoostBeastHttpClient` - HTTP-клиент для скачивания страниц
- `BoostBeastHttpServer` - HTTP-сервер для обработки запросов
- `HtmlParser` - парсинг HTML-страниц
- `TextProcessor` - обработка текста (Boost Locale)
- `IniConfiguration` - чтение конфигурации из INI-файлов

↓ *реализуют порты из*

**Layer 4: Application Layer (Бизнес-сценарии)**

*Use Cases (варианты использования):*
- `IndexPageUseCase` - индексация веб-страницы
- `SearchDocumentsUseCase` - поиск по документам

*Ports (интерфейсы):*
- `IDocumentRepository` - интерфейс репозитория документов
- `IWordRepository` - интерфейс репозитория слов
- `IHttpClient` - интерфейс HTTP-клиента
- `IHttpServer` - интерфейс HTTP-сервера
- `IHtmlParser` - интерфейс парсера HTML
- `ITextProcessor` - интерфейс обработки текста
- `IConfiguration` - интерфейс конфигурации

↓ *оркеструют работу*

**Layer 5: Domain Layer (Бизнес-логика)**

*Entities (сущности):*
- `Document` - веб-страница
- `Word` - уникальное слово
- `WordFrequency` - связь документ-слово с частотой
- `SearchResult` - результат поиска

*Value Objects (объекты-значения):*
- `Url` - валидированный URL
- `SearchQuery` - поисковый запрос

*Domain Services (доменные сервисы):*
- `IndexingService` - анализ частотности слов
- `RankingService` - ранжирование результатов
