#include "IniConfiguration.h"

#include <fstream>
#include <stdexcept>

namespace Infrastructure::Configuration {

IniConfiguration::IniConfiguration(const std::string& filePath) {
    load(filePath);
}

void IniConfiguration::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл конфигурации: " + filePath);
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        // Убираем пробелы
        line = trim(line);

        // Пропускаем пустые строки и комментарии
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        // Проверяем, является ли строка секцией [section]
        static constexpr char SECTION_START = '[';
        static constexpr char SECTION_END = ']';
        static constexpr size_t SECTION_BRACKET_SIZE = 2;

        if (line[0] == SECTION_START && line[line.length() - 1] == SECTION_END) {
            currentSection = line.substr(1, line.length() - SECTION_BRACKET_SIZE);
            currentSection = trim(currentSection);
            continue;
        }

        // Парсим пару key=value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Сохраняем в карту
            data_[currentSection][key] = value;
        }
    }
}

std::string IniConfiguration::getValue(const std::string& section,
                                       const std::string& key,
                                       const std::string& defaultValue) const {
    auto sectionIt = data_.find(section);
    if (sectionIt == data_.end()) {
        return defaultValue;
    }

    auto keyIt = sectionIt->second.find(key);
    if (keyIt == sectionIt->second.end()) {
        return defaultValue;
    }

    return keyIt->second;
}

int IniConfiguration::getIntValue(const std::string& section, const std::string& key, int defaultValue) const {
    std::string value = getValue(section, key);
    if (value.empty()) {
        return defaultValue;
    }

    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

std::string IniConfiguration::trim(const std::string& str) {
    static constexpr char WHITESPACE_CHARS[] = " \t\n\r";
    static constexpr size_t SUBSTRING_OFFSET = 1;

    size_t start = str.find_first_not_of(WHITESPACE_CHARS);
    if (start == std::string::npos) {
        return "";
    }

    size_t end = str.find_last_not_of(WHITESPACE_CHARS);
    return str.substr(start, end - start + SUBSTRING_OFFSET);
}

// Настройки базы данных
std::string IniConfiguration::getDatabaseHost() const {
    return getValue("database", "host", "localhost");
}

int IniConfiguration::getDatabasePort() const {
    return getIntValue("database", "port", DEFAULT_DATABASE_PORT);
}

std::string IniConfiguration::getDatabaseName() const {
    return getValue("database", "dbname", "search_system");
}

std::string IniConfiguration::getDatabaseUser() const {
    return getValue("database", "user", "postgres");
}

std::string IniConfiguration::getDatabasePassword() const {
    return getValue("database", "password", "");
}

// Настройки Spider
std::string IniConfiguration::getSpiderStartUrl() const {
    return getValue("spider", "start_url", "https://example.com");
}

int IniConfiguration::getSpiderCrawlDepth() const {
    return getIntValue("spider", "crawl_depth", DEFAULT_SPIDER_CRAWL_DEPTH);
}

int IniConfiguration::getSpiderThreadPoolSize() const {
    return getIntValue("spider", "thread_pool_size", DEFAULT_SPIDER_THREAD_POOL_SIZE);
}

// Настройки HTTP Server
int IniConfiguration::getHttpServerPort() const {
    return getIntValue("http_server", "port", DEFAULT_HTTP_SERVER_PORT);
}

int IniConfiguration::getHttpServerMaxResults() const {
    return getIntValue("http_server", "max_results", DEFAULT_HTTP_SERVER_MAX_RESULTS);
}

} // namespace Infrastructure::Configuration
