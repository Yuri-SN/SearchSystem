#include <iostream>
#include <sstream>
#include <string>

#include "../HTTPServerData/DIContainer.h"

/**
 * @brief Генерирует HTML-страницу с формой поиска
 */
std::string generateSearchFormHtml() {
    return R"(<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Поисковая система</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .search-box {
            text-align: center;
            margin-top: 50px;
        }
        input[type="text"] {
            width: 60%;
            padding: 12px;
            font-size: 16px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        button {
            padding: 12px 30px;
            font-size: 16px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            margin-left: 10px;
        }
        button:hover {
            background-color: #45a049;
        }
        .info {
            text-align: center;
            margin-top: 20px;
            color: #666;
            font-size: 14px;
        }
    </style>
</head>
<body>
    <h1>Поисковая система</h1>
    <div class="search-box">
        <form method="POST" action="/search">
            <input type="text" name="query" placeholder="Введите поисковый запрос..." autofocus>
            <button type="submit">Найти</button>
        </form>
    </div>
    <div class="info">
        <p>Максимум 4 слова в запросе</p>
    </div>
</body>
</html>)";
}

/**
 * @brief Генерирует HTML-страницу с результатами поиска
 */
std::string generateSearchResultsHtml(const std::vector<Core::Domain::Model::SearchResult>& results,
                                      const std::string& query) {
    std::ostringstream html;

    html << R"(<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Результаты поиска: )" << query
         << R"(</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
        }
        h1 {
            color: #333;
        }
        .search-again {
            margin-bottom: 30px;
        }
        input[type="text"] {
            width: 60%;
            padding: 12px;
            font-size: 16px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        button {
            padding: 12px 30px;
            font-size: 16px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            margin-left: 10px;
        }
        button:hover {
            background-color: #45a049;
        }
        .result {
            margin-bottom: 25px;
            padding: 15px;
            background-color: #f9f9f9;
            border-radius: 4px;
        }
        .result-title {
            font-size: 18px;
            margin-bottom: 5px;
        }
        .result-title a {
            color: #1a0dab;
            text-decoration: none;
        }
        .result-title a:hover {
            text-decoration: underline;
        }
        .result-relevance {
            color: #666;
            font-size: 14px;
        }
        .no-results {
            text-align: center;
            color: #666;
            margin-top: 50px;
            font-size: 18px;
        }
        .results-info {
            color: #666;
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <h1>Результаты поиска</h1>
    <div class="search-again">
        <form method="POST" action="/search">
            <input type="text" name="query" value=")" << query
         << R"(" autofocus>
            <button type="submit">Найти</button>
        </form>
    </div>
)";

    if (results.empty()) {
        html << R"(
    <div class="no-results">
        <p>По вашему запросу ничего не найдено.</p>
        <p>Попробуйте изменить запрос.</p>
    </div>
)";
    } else {
        html << "    <div class=\"results-info\">Найдено результатов: " << results.size() << "</div>\n";

        for (const auto& result : results) {
            html << "    <div class=\"result\">\n"
                 << "        <div class=\"result-title\"><a href=\"" << result.getUrl() << "\" target=\"_blank\">"
                 << result.getUrl() << "</a></div>\n"
                 << "        <div class=\"result-relevance\">Релевантность: " << result.getRelevance()
                 << "</div>\n"
                 << "    </div>\n";
        }
    }

    html << R"(
</body>
</html>)";

    return html.str();
}

/**
 * @brief Генерирует HTML-страницу с сообщением об ошибке
 */
std::string generateErrorHtml(const std::string& errorMessage) {
    std::ostringstream html;

    html << R"(<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ошибка</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            text-align: center;
        }
        h1 {
            color: #d32f2f;
        }
        .error-message {
            background-color: #ffebee;
            padding: 20px;
            border-radius: 4px;
            margin: 30px 0;
            color: #c62828;
        }
        a {
            color: #1a0dab;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <h1>Ошибка</h1>
    <div class="error-message">
        <p>)" << errorMessage
         << R"(</p>
    </div>
    <p><a href="/">Вернуться к поиску</a></p>
</body>
</html>)";

    return html.str();
}

/**
 * @brief Парсит тело POST-запроса для извлечения параметра query
 */
std::string parseQueryFromBody(const std::string& body) {
    // Формат: query=search+terms или query=search%20terms
    const std::string prefix = "query=";
    size_t pos = body.find(prefix);

    if (pos == std::string::npos) {
        return "";
    }

    std::string query = body.substr(pos + prefix.length());

    // Находим конец значения (до & или конца строки)
    size_t endPos = query.find('&');
    if (endPos != std::string::npos) {
        query = query.substr(0, endPos);
    }

    // URL-декодирование: заменяем + на пробелы
    std::string decoded;
    for (size_t i = 0; i < query.length(); ++i) {
        if (query[i] == '+') {
            decoded += ' ';
        } else if (query[i] == '%' && i + 2 < query.length()) {
            // Простое декодирование %20 -> пробел
            if (query.substr(i, 3) == "%20") {
                decoded += ' ';
                i += 2;
            } else {
                decoded += query[i];
            }
        } else {
            decoded += query[i];
        }
    }

    return decoded;
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Поисковая система - HTTP Сервер ===" << std::endl;
        std::cout << std::endl;

        // Определяем путь к конфигурации
        std::string configPath = "config.ini";
        if (argc > 1) {
            configPath = argv[1];
        }

        std::cout << "Загрузка конфигурации из: " << configPath << std::endl;

        // Создаём DI контейнер
        HTTPServerData::DIContainer container(configPath);

        // Получаем конфигурацию
        auto config = container.getConfiguration();
        const int port = config->getHttpServerPort();
        const int maxResults = config->getHttpServerMaxResults();

        std::cout << "Порт HTTP-сервера: " << port << std::endl;
        std::cout << "Максимум результатов: " << maxResults << std::endl;
        std::cout << std::endl;

        // Получаем зависимости
        auto searchDocumentsUseCase = container.getSearchDocumentsUseCase();
        auto httpServer = container.getHttpServer();

        // Обработчик HTTP-запросов
        auto requestHandler = [searchDocumentsUseCase, maxResults](const std::string& method,
                                                                    const std::string& target,
                                                                    const std::string& body) -> std::string {
            try {
                // GET / - форма поиска
                if (method == "GET" && (target == "/" || target == "/search")) {
                    return generateSearchFormHtml();
                }

                // POST /search - выполнение поиска
                if (method == "POST" && target == "/search") {
                    // Извлекаем запрос из тела
                    std::string queryString = parseQueryFromBody(body);

                    if (queryString.empty()) {
                        return generateErrorHtml("Пустой поисковый запрос");
                    }

                    // Создаём SearchQuery
                    auto searchQuery = Core::Domain::ValueObject::SearchQuery::create(queryString);

                    if (!searchQuery.has_value()) {
                        return generateErrorHtml(
                            "Некорректный запрос. Максимум 4 слова, разделённых пробелами.");
                    }

                    // Выполняем поиск
                    auto results = searchDocumentsUseCase->execute(searchQuery.value(), maxResults);

                    // Возвращаем HTML с результатами
                    return generateSearchResultsHtml(results, queryString);
                }

                // Неизвестный запрос
                return generateErrorHtml("Страница не найдена");

            } catch (const std::exception& e) {
                std::cerr << "Ошибка обработки запроса: " << e.what() << std::endl;
                return generateErrorHtml("Внутренняя ошибка сервера: " + std::string(e.what()));
            }
        };

        std::cout << "Запуск HTTP-сервера на порту " << port << "..." << std::endl;
        std::cout << "Откройте браузер: http://localhost:" << port << "/" << std::endl;
        std::cout << "Для остановки нажмите Ctrl+C" << std::endl;
        std::cout << std::endl;

        // Запускаем сервер (блокирующий вызов)
        httpServer->start(port, requestHandler);

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: " << e.what() << std::endl;
        return 1;
    }
}
