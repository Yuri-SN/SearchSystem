#pragma once

#include <string>

namespace Core::Ports {
/**
 * @brief Интерфейс для работы с конфигурацией приложения
 *
 * Порт для доступа к настройкам из ini-файла.
 * Реализация будет в Infrastructure слое.
 */
class IConfiguration {
  public:
    virtual ~IConfiguration() = default;

    // Настройки базы данных
    virtual std::string getDatabaseHost() const = 0;
    virtual int getDatabasePort() const = 0;
    virtual std::string getDatabaseName() const = 0;
    virtual std::string getDatabaseUser() const = 0;
    virtual std::string getDatabasePassword() const = 0;

    // Настройки Spider
    virtual std::string getSpiderStartUrl() const = 0;
    virtual int getSpiderCrawlDepth() const = 0;
    virtual int getSpiderThreadPoolSize() const = 0;

    // Настройки HTTP Server
    virtual int getHttpServerPort() const = 0;
    virtual int getHttpServerMaxResults() const = 0;
};
} // namespace Core::Ports
