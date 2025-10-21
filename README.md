# Дипломный проект «Поисковая система»

Поисковая система на C++ с использованием чистой архитектуры.

## Описание проекта

Поисковая система состоит из двух приложений:

1. **Spider** - программа-краулер для индексации веб-страниц
2. **HTTPServer** - HTTP-сервер для обработки поисковых запросов

## Архитектура

Проект построен по принципам **Clean Architecture** с разделением на слои:

```bash
┌─────────────────────────────────────────────────────────────┐
│                  Приложения (Entry Points)                  │
│                                                             │
│    ┌──────────────────┐             ┌──────────────────┐    │
│    │     Spider       │             │   HTTPServer     │    │
│    │   (main.cpp)     │             │   (main.cpp)     │    │
│    └────────┬─────────┘             └────────┬─────────┘    │
│             │                                │              │
└─────────────┼────────────────────────────────┼──────────────┘
              │                                │
              ↓                                ↓
┌─────────────────────────────────────────────────────────────┐
│                DI Containers (Composition Root)             │
│                                                             │
│    ┌──────────────────┐             ┌──────────────────┐    │
│    │   SpiderData     │             │ HTTPServerData   │    │
│    │  (DI Container)  │             │  (DI Container)  │    │
│    └────────┬─────────┘             └────────┬─────────┘    │
└─────────────┼────────────────────────────────┼──────────────┘
              │                                │
              └────────────────┬───────────────┘
                               ↓
┌─────────────────────────────────────────────────────────────┐
│                     Infrastructure Layer                    │
│                                                             │
│   ┌─────────────────────────────────────────────────────┐   │
│   │                    Adapters                         │   │
│   │  • PostgresDocumentRepository                       │   │
│   │  • PostgresWordRepository                           │   │
│   │  • BoostBeastHttpClient                             │   │
│   │  • BoostBeastHttpServer                             │   │
│   │  • HtmlParser                                       │   │
│   │  • TextProcessor (Boost Locale)                     │   │
│   │  • IniConfiguration                                 │   │
│   └─────────────────────────────────────────────────────┘   │
└──────────────────────────────┬──────────────────────────────┘
                               ↓
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                      │
│                                                             │
│   ┌──────────────────┐               ┌──────────────────┐   │
│   │   Use Cases      │               │     Ports        │   │
│   │                  │               │  (Interfaces)    │   │
│   │ • IndexPage      │               │ • IDocRepository │   │
│   │ • SearchDocuments│               │ • IWordRepository│   │
│   │                  │               │ • IHttpClient    │   │
│   │                  │               │ • IConfiguration │   │
│   │                  │               │ • etc...         │   │
│   └──────────────────┘               └──────────────────┘   │
└──────────────────────────────┬──────────────────────────────┘
                               ↓
┌─────────────────────────────────────────────────────────────┐
│                         Domain Layer                        │
│        (Бизнес-логика, не зависит от внешних систем)        │
│                                                             │
│   ┌──────────────┐   ┌──────────────┐   ┌──────────────┐    │
│   │   Entities   │   │Value Objects │   │   Services   │    │
│   │              │   │              │   │              │    │
│   │ • Document   │   │ • Url        │   │ • Indexing   │    │
│   │ • Word       │   │ • SearchQuery│   │ • Ranking    │    │
│   │ • WordFreq   │   │              │   │              │    │
│   │ • SearchRes  │   │              │   │              │    │
│   └──────────────┘   └──────────────┘   └──────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

## Структура проекта

```bash
SearchSystem/
├── Core/                           # Ядро системы (Domain + Application)
│   ├── Domain/                     # Доменный слой
│   │   ├── Model/                  # Сущности
│   │   │   ├── Document.h/cpp
│   │   │   ├── Word.h/cpp
│   │   │   ├── WordFrequency.h/cpp
│   │   │   └── SearchResult.h/cpp
│   │   ├── ValueObject/            # Объекты-значения
│   │   │   ├── Url.h/cpp
│   │   │   └── SearchQuery.h/cpp
│   │   └── Service/                # Доменные сервисы
│   │       ├── IndexingService.h/cpp
│   │       └── RankingService.h/cpp
│   ├── Application/                # Слой приложения
│   │   └── UseCases/               # Варианты использования
│   │       ├── IndexPageUseCase.h/cpp
│   │       └── SearchDocumentsUseCase.h/cpp
│   ├── Ports/                      # Интерфейсы (порты)
│   │   ├── IDocumentRepository.h
│   │   ├── IWordRepository.h
│   │   ├── IHttpClient.h
│   │   ├── IHttpServer.h
│   │   ├── IHtmlParser.h
│   │   ├── ITextProcessor.h
│   │   └── IConfiguration.h
│   ├── DTO/                        # Data Transfer Objects
│   │   ├── CrawlResultDto.h
│   │   ├── SearchRequestDto.h
│   │   └── SearchResponseDto.h
│   └── CMakeLists.txt              # → libCore.a
│
├── Infrastructure/                 # Слой инфраструктуры (адаптеры)
│   ├── Adapters/                   # Реализации портов
│   │   ├── PostgresRepository.cpp
│   │   └── ...
│   ├── DTO/                        # DTO для инфраструктуры
│   └── CMakeLists.txt              # → libInfrastructure.a
│
├── Spider/                         # Приложение "Паук"
│   ├── main.cpp                    # Точка входа
│   └── CMakeLists.txt              # → Spider (executable)
│
├── SpiderData/                     # DI-контейнер для Spider
│   ├── DIContainer.cpp             # Композиция зависимостей
│   └── CMakeLists.txt              # → libSpiderData.a
│
├── HTTPServer/                     # Приложение "Поисковик"
│   ├── main.cpp                    # Точка входа
│   └── CMakeLists.txt              # → HTTPServer (executable)
│
├── HTTPServerData/                 # DI-контейнер для HTTPServer
│   ├── DIContainer.cpp             # Композиция зависимостей
│   └── CMakeLists.txt              # → libHTTPServerData.a
│
├── CommonFiles/                    # Вспомогательные файлы
│   ├── config.ini                  # Конфигурационный файл
│   └── README.md                   # Этот файл
│
└── CMakeLists.txt                  # Корневой CMake
```

## Граф зависимостей

```bash
Spider (executable)
  └─> libSpiderData.a
       ├─> libCore.a
       └─> libInfrastructure.a
            ├─> libCore.a
            ├─> Boost (locale, system, thread; beast - header-only)
            └─> libpqxx (PostgreSQL)

HTTPServer (executable)
  └─> libHTTPServerData.a
       ├─> libCore.a
       └─> libInfrastructure.a
            ├─> libCore.a
            ├─> Boost (locale, system, thread; beast - header-only)
            └─> libpqxx (PostgreSQL)
```

## Принципы Clean Architecture

### 1. Независимость от фреймворков

- Core не зависит от Boost, PostgreSQL или других библиотек
- Можно легко заменить Boost Beast на другой HTTP-фреймворк

### 2. Тестируемость

- Бизнес-логика в Domain изолирована
- Use Cases легко тестируются через моки портов

### 3. Независимость от UI

- Логика не зависит от способа взаимодействия (HTTP, CLI, GUI)

### 4. Независимость от БД

- Можно переключиться с PostgreSQL на MySQL или MongoDB
- Репозитории реализуют абстрактные интерфейсы

### 5. Правило зависимостей

```bash
Infrastructure → Application → Domain
     ↓               ↓           ↓
  (адаптеры)    (use cases)  (сущности)
```

Зависимости направлены **внутрь**: внешние слои знают о внутренних, но не наоборот.

## Технологии

- **Язык:** C++17
- **Сборка:** CMake 3.16+
- **База данных:** PostgreSQL (libpqxx 7.10+)
- **HTTP:** Boost Beast (Boost 1.88+)
- **Локализация:** Boost Locale
- **Конфигурация:** INI-файлы

## Компоненты системы

### Core Library (libCore.a)

**Доменный слой:**

- `Document` - представление веб-страницы
- `Word` - уникальное слово в системе
- `WordFrequency` - связь документ-слово с частотой
- `SearchResult` - результат поиска с релевантностью
- `Url` - валидированный URL (value object)
- `SearchQuery` - поисковый запрос (value object)
- `IndexingService` - анализ частотности слов
- `RankingService` - ранжирование результатов

**Слой приложения:**

- `IndexPageUseCase` - индексация веб-страницы
- `SearchDocumentsUseCase` - поиск документов
- Порты (интерфейсы) для репозиториев, HTTP, парсеров

### Infrastructure Library (libInfrastructure.a)

Реализации адаптеров для:

- Работы с PostgreSQL
- HTTP-клиента (Boost Beast)
- HTTP-сервера (Boost Beast)
- HTML-парсинга
- Обработки текста (lowercase, нормализация)
- Чтения INI-конфигурации

### Spider (приложение-краулер)

**Функции:**

1. Скачивает веб-страницы начиная с `start_url`
2. Переходит по ссылкам до заданной глубины рекурсии
3. Парсит HTML и извлекает текст
4. Анализирует частотность слов
5. Сохраняет индексы в PostgreSQL
6. Использует пул потоков для параллельной обработки

**Конфигурация (config.ini):**

```ini
[spider]
start_url=https://example.com
crawl_depth=3
thread_pool_size=10
```

### HTTPServer (поисковик)

**Функции:**

1. Принимает HTTP GET запросы - возвращает форму поиска
2. Принимает HTTP POST запросы - выполняет поиск и возвращает результаты
3. Ранжирует результаты по релевантности (сумма частот слов)
4. Ограничивает выдачу (по умолчанию 10 результатов)

**Конфигурация (config.ini):**

```ini
[http_server]
port=8080
max_results=10
```

## База данных

### Схема таблиц

```sql
-- Таблица документов
CREATE TABLE IF NOT EXISTS documents (
    id BIGSERIAL PRIMARY KEY,
    url VARCHAR(2048) UNIQUE NOT NULL,
    content TEXT NOT NULL
);

-- Таблица слов
CREATE TABLE IF NOT EXISTS words (
    id BIGSERIAL PRIMARY KEY,
    text VARCHAR(32) UNIQUE NOT NULL
);

-- Таблица частотности (связь многие-ко-многим)
CREATE TABLE IF NOT EXISTS word_frequencies (
    document_id BIGINT REFERENCES documents(id) ON DELETE CASCADE,
    word_id BIGINT REFERENCES words(id) ON DELETE CASCADE,
    frequency INTEGER NOT NULL,
    PRIMARY KEY (document_id, word_id)
);

-- Индексы для ускорения поиска
CREATE INDEX IF NOT EXISTS idx_words_text ON words(text);
CREATE INDEX IF NOT EXISTS idx_word_frequencies_word_id ON word_frequencies(word_id);
```

### Конфигурация БД (config.ini)

```ini
[database]
host=localhost
port=5432
dbname=search_system
user=postgres
password=your_password
```

## Требования

- C++17 или выше
- CMake 3.16+
- Boost 1.88+ (system, locale, beast)
- PostgreSQL 12+
- libpqxx 7.10+

## Сборка проекта

```bash
# Установка зависимостей (Ubuntu/Debian)
sudo apt install libboost-locale-dev libboost-system-dev libboost-thread-dev libpqxx-dev libpq-dev postgresql

# Создание директории для сборки
mkdir -p build && cd build

# Конфигурация CMake
cmake ..

# Сборка всех компонентов
make -j4

# Или сборка отдельных компонентов
make Core              # libCore.a
make Infrastructure    # libInfrastructure.a
make SpiderData        # libSpiderData.a
make HTTPServerData    # libHTTPServerData.a
make Spider            # Spider (executable)
make HTTPServer        # HTTPServer (executable)
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

### 3. Запуск Spider

```bash
./build/Spider
```

Spider начнёт индексацию с `start_url` и сохранит данные в БД.

### 4. Запуск HTTPServer

```bash
./build/HTTPServer
```

Поисковик будет доступен по адресу `http://localhost:8080`

## Использование

### Поиск через веб-интерфейс

1. Откройте браузер: `http://localhost:8080`
2. Введите поисковый запрос (до 4 слов)
3. Получите отсортированные по релевантности результаты

### Алгоритм ранжирования

Релевантность документа = сумма частот всех искомых слов в документе

**Пример:**

- Запрос: "привет мир"
- Документ A: "привет" встречается 10 раз, "мир" - 4 раза → релевантность = 14
- Документ B: "привет" встречается 3 раза, "мир" - 8 раз → релевантность = 11
- Результат: Документ A выше в выдаче
