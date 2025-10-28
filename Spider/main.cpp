#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <vector>

#include <windows.h>

#include "../Infrastructure/Http/BoostBeastHttpClient.h"
#include "../Infrastructure/Parsers/HtmlParser.h"
#include "../SpiderData/DIContainer.h"

/**
 * @brief Многопоточная очередь URL для краулинга
 */
class CrawlQueue {
  public:
    /**
     * @brief Добавляет URL в очередь с указанной глубиной
     */
    void push(const std::string& url, int depth) {
        std::lock_guard<std::mutex> lock(mutex_);

        // Проверяем, не обрабатывали ли мы уже этот URL
        if (visited_.count(url) > 0) {
            return;
        }

        queue_.push({url, depth});
        visited_.insert(url);
        cv_.notify_one();
    }

    /**
     * @brief Извлекает URL из очереди (блокирующая операция)
     * @return Пара {url, depth} или nullopt если очередь пуста и работа завершена
     */
    std::optional<std::pair<std::string, int>> pop() {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(lock, [this] { return !queue_.empty() || done_; });

        if (queue_.empty()) {
            return std::nullopt;
        }

        auto item = queue_.front();
        queue_.pop();
        activeCount_++;

        return item;
    }

    /**
     * @brief Отмечает завершение обработки одного URL
     */
    void markCompleted() {
        std::lock_guard<std::mutex> lock(mutex_);
        activeCount_--;

        // Если очередь пуста и нет активных задач - работа завершена
        if (queue_.empty() && activeCount_ == 0) {
            done_ = true;
            cv_.notify_all();
        }
    }

    /**
     * @brief Проверяет, завершена ли работа
     */
    bool isDone() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return done_ && queue_.empty() && activeCount_ == 0;
    }

    /**
     * @brief Получить количество обработанных URL
     */
    size_t getVisitedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return visited_.size();
    }

  private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::pair<std::string, int>> queue_;  // {url, depth}
    std::set<std::string> visited_;
    int activeCount_ = 0;
    bool done_ = false;
};

/**
 * @brief Рабочий поток краулера
 */
class CrawlerWorker {
  public:
    CrawlerWorker(int workerId, std::shared_ptr<CrawlQueue> queue,
                  std::shared_ptr<Core::Application::UseCases::IndexPageUseCase> indexPageUseCase,
                  std::shared_ptr<Core::Ports::IHttpClient> httpClient,
                  std::shared_ptr<Core::Ports::IHtmlParser> htmlParser, int maxDepth)
        : workerId_(workerId),
          queue_(std::move(queue)),
          indexPageUseCase_(std::move(indexPageUseCase)),
          httpClient_(std::move(httpClient)),
          htmlParser_(std::move(htmlParser)),
          maxDepth_(maxDepth) {
        // Устанавливаем ID потока для логирования в HTTP клиенте
        httpClient_->setWorkerId(workerId_);
    }

    void run() {
        while (true) {
            auto item = queue_->pop();

            if (!item.has_value()) {
                break;  // Очередь пуста и работа завершена
            }

            const auto& [url, depth] = item.value();

            try {
                processUrl(url, depth);
            } catch (const std::exception& e) {
                std::cerr << "[Поток " << workerId_ << "] Ошибка при обработке " << url << ": " << e.what() << "\n";
            }

            queue_->markCompleted();
        }
    }

  private:
    void processUrl(const std::string& url, int depth) {
        std::cout << "[Поток " << workerId_ << "] Обработка [глубина " << depth << "]: " << url << "\n";

        // Скачиваем страницу
        auto htmlContent = httpClient_->get(url);

        if (!htmlContent.has_value()) {
            std::cerr << "[Поток " << workerId_ << "] Не удалось скачать: " << url << "\n";
            return;
        }

        // Индексируем страницу
        const auto documentId = indexPageUseCase_->execute(url, htmlContent.value());

        if (documentId == 0) {
            std::cerr << "[Поток " << workerId_ << "] Не удалось проиндексировать: " << url << "\n";
            return;
        }

        std::cout << "[Поток " << workerId_ << "] Проиндексирован документ ID=" << documentId << ": " << url << "\n";

        // Если не достигли максимальной глубины - извлекаем ссылки
        if (depth < maxDepth_) {
            auto links = htmlParser_->extractLinks(htmlContent.value(), url);

            std::cout << "[Поток " << workerId_ << "] Найдено ссылок: " << links.size() << " на странице " << url
                      << "\n";

            for (const auto& link : links) {
                queue_->push(link, depth + 1);
            }
        }
    }

    int workerId_;
    std::shared_ptr<CrawlQueue> queue_;
    std::shared_ptr<Core::Application::UseCases::IndexPageUseCase> indexPageUseCase_;
    std::shared_ptr<Core::Ports::IHttpClient> httpClient_;
    std::shared_ptr<Core::Ports::IHtmlParser> htmlParser_;
    int maxDepth_;
};

int main(int argc, char* argv[]) {
    // Устанавливаем UTF-8 для консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        std::cout << "=== Поисковая система - Программа Паук ===" << "\n\n";

        // Определяем путь к конфигурации
        std::string configPath = "config.ini";
        if (argc > 1) {
            configPath = argv[1];
        }

        std::cout << "Загрузка конфигурации из: " << configPath << "\n";

        // Создаём DI контейнер
        SpiderData::DIContainer container(configPath);

        // Получаем конфигурацию
        auto config = container.getConfiguration();
        const std::string startUrl = config->getSpiderStartUrl();
        const int maxDepth = config->getSpiderCrawlDepth();
        const int threadPoolSize = config->getSpiderThreadPoolSize();

        std::cout << "Стартовый URL: " << startUrl << "\n";
        std::cout << "Глубина рекурсии: " << maxDepth << "\n";
        std::cout << "Размер пула потоков: " << threadPoolSize << "\n";
        std::cout << "\n";

        // Создаём многопоточную очередь
        auto queue = std::make_shared<CrawlQueue>();

        // Добавляем стартовый URL
        queue->push(startUrl, 1);

        // Создаём пул потоков
        std::vector<std::thread> threads;
        threads.reserve(threadPoolSize);

        std::cout << "Запуск " << threadPoolSize << " потоков краулера...\n";
        std::cout << "\n";

        // ВАЖНО: Каждый поток должен использовать свой собственный IndexPageUseCase
        // с отдельным подключением к БД, чтобы избежать конфликтов транзакций

        // Запускаем рабочие потоки
        for (int i = 0; i < threadPoolSize; ++i) {
            threads.emplace_back([&container, queue, maxDepth, workerId = i + 1]() {
                // Каждый поток создаёт свой собственный IndexPageUseCase
                // с отдельным подключением к БД
                auto indexPageUseCase = container.createIndexPageUseCase();
                auto httpClient = std::make_shared<Infrastructure::Http::BoostBeastHttpClient>();
                auto htmlParser = std::make_shared<Infrastructure::Parsers::HtmlParser>();

                CrawlerWorker worker(workerId, queue, indexPageUseCase, httpClient, htmlParser, maxDepth);
                worker.run();
            });
        }

        // Ждём завершения всех потоков
        for (auto& thread : threads) {
            thread.join();
        }

        std::cout << "\n";
        std::cout << "=== Краулинг завершён ===" << "\n";
        std::cout << "Всего обработано URL: " << queue->getVisitedCount() << "\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: " << e.what() << "\n";
        return 1;
    }
}
