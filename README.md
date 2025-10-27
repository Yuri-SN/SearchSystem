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

*Models (модели):*
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

## Граф зависимостей

```bash
Spider (executable)
  └─> libSpiderData.a
       ├─> libCore.a
       └─> libInfrastructure.a
            ├─> libCore.a
            ├─> Boost (locale, system, thread)
            ├─> OpenSSL (ssl, crypto) - для HTTPS
            ├─> libpqxx (PostgreSQL)
            └─> gumbo-parser (HTML парсинг)

HTTPServer (executable)
  └─> libHTTPServerData.a
       ├─> libCore.a
       └─> libInfrastructure.a
            ├─> libCore.a
            ├─> Boost (locale, system, thread)
            ├─> OpenSSL (ssl, crypto) - для HTTPS
            ├─> libpqxx (PostgreSQL)
            └─> gumbo-parser (HTML парсинг)
```

## Технологии

- **Язык:** C++17
- **Сборка:** CMake 3.16+
- **База данных:** PostgreSQL (libpqxx 7.10+)
- **HTTP:** Boost Beast (Boost 1.88+)
- **SSL/TLS:** OpenSSL 3.5+ (для HTTPS)
- **Локализация:** Boost Locale
- **HTML парсинг:** gumbo-parser 0.13+
- **Конфигурация:** INI-файлы

## Установка зависимостей

### Windows

**Используя vcpkg:**

```powershell
# Установка vcpkg (если ещё не установлен)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Установка зависимостей
.\vcpkg install boost-locale boost-system boost-thread boost-asio boost-beast openssl libpqxx gumbo

# Интеграция с Visual Studio
.\vcpkg integrate install
```

**Используя CMake с vcpkg:**

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[путь к vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Сборка проекта

### Windows

```powershell
# Создание директории для сборки
mkdir build
cd build

# Конфигурация CMake (с vcpkg)
cmake .. -DCMAKE_TOOLCHAIN_FILE=[путь к vcpkg]/scripts/buildsystems/vcpkg.cmake

# Сборка
cmake --build . --config Release
```

## Запуск

### 1. Настройка базы данных

```bash
# Создание базы данных
createdb search_system

# Таблицы создадутся автоматически при первом запуске Spider
```

### 2. Настройка конфигурации

Отредактируйте `config.ini` в корне проекта:

```ini
[database]
host=localhost
port=5432
dbname=search_system
user=postgres
password=secret

[spider]
start_url=https://example.com
crawl_depth=3
thread_pool_size=10

[http_server]
port=8080
max_results=10
```

### 3. Запуск Spider (краулера)

```bash
./build/Spider/Spider
```

### 4. Запуск HTTPServer (поисковика)

```bash
./build/HTTPServer/HTTPServer
```

Поисковик будет доступен по адресу `http://localhost:8080`
