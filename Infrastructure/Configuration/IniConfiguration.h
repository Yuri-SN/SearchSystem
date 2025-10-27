#pragma once

#include <map>
#include <string>

#include "../../Core/Ports/IConfiguration.h"

namespace Infrastructure::Configuration {

/**
 * @brief Реализация IConfiguration для чтения INI файлов
 *
 * Парсит простые INI файлы формата:
 * [section]
 * key=value
 */
class IniConfiguration : public Core::Ports::IConfiguration {
  public:
    /**
     * @brief Конструктор
     * @param filePath Путь к INI файлу
     */
    explicit IniConfiguration(const std::string& filePath);

    // Настройки базы данных
    std::string getDatabaseHost() const override;
    int getDatabasePort() const override;
    std::string getDatabaseName() const override;
    std::string getDatabaseUser() const override;
    std::string getDatabasePassword() const override;

    // Настройки Spider
    std::string getSpiderStartUrl() const override;
    int getSpiderCrawlDepth() const override;
    int getSpiderThreadPoolSize() const override;

    // Настройки HTTP Server
    int getHttpServerPort() const override;
    int getHttpServerMaxResults() const override;

  private:
    // Константы значений по умолчанию
    static constexpr int DEFAULT_DATABASE_PORT = 5432;
    static constexpr int DEFAULT_SPIDER_CRAWL_DEPTH = 3;
    static constexpr int DEFAULT_SPIDER_THREAD_POOL_SIZE = 10;
    static constexpr int DEFAULT_HTTP_SERVER_PORT = 8080;
    static constexpr int DEFAULT_HTTP_SERVER_MAX_RESULTS = 10;

    /**
     * @brief Загружает и парсит INI файл
     */
    void load(const std::string& filePath);

    /**
     * @brief Получает значение из конфигурации
     * @param section Секция в INI файле
     * @param key Ключ
     * @param defaultValue Значение по умолчанию
     * @return Значение из конфигурации или defaultValue
     */
    std::string getValue(const std::string& section,
                         const std::string& key,
                         const std::string& defaultValue = "") const;

    /**
     * @brief Получает числовое значение из конфигурации
     * @param section Секция в INI файле
     * @param key Ключ
     * @param defaultValue Значение по умолчанию
     * @return Числовое значение или defaultValue
     */
    int getIntValue(const std::string& section, const std::string& key, int defaultValue = 0) const;

    /**
     * @brief Убирает пробелы в начале и конце строки
     */
    static std::string trim(const std::string& str);

    // Хранилище: [section][key] = value
    std::map<std::string, std::map<std::string, std::string>> data_;
};

} // namespace Infrastructure::Configuration
