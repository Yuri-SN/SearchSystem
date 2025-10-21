#pragma once

#include <memory>

#include "../../Core/Ports/IWordRepository.h"
#include "DatabaseConnection.h"

namespace Infrastructure::Database {

/**
 * @brief PostgreSQL реализация репозитория слов и частотности
 *
 * Работает с таблицами words и word_frequencies.
 * Реализует операции сохранения слов, частотности и сложные поисковые запросы.
 */
class PostgresWordRepository : public Core::Ports::IWordRepository {
  public:
    /**
     * @brief Конструктор
     * @param dbConnection Соединение с базой данных
     */
    explicit PostgresWordRepository(std::shared_ptr<DatabaseConnection> dbConnection);

    ~PostgresWordRepository() override = default;

    /**
     * @brief Сохраняет слово в базе данных
     * @param word Слово для сохранения
     * @return ID сохраненного слова
     *
     * Если слово с таким текстом уже существует, возвращает его ID.
     * Если слово новое, вставляет новую запись.
     */
    Core::Domain::Model::Word::IdType save(Core::Domain::Model::Word& word) override;

    /**
     * @brief Находит слово по тексту
     * @param text Текст слова
     * @return Слово, если найдено
     */
    std::optional<Core::Domain::Model::Word> findByText(const std::string& text) override;

    /**
     * @brief Сохраняет частотность слова в документе
     * @param frequency Частотность для сохранения
     *
     * Использует INSERT ... ON CONFLICT для обновления существующей частоты.
     */
    void saveFrequency(const Core::Domain::Model::WordFrequency& frequency) override;

    /**
     * @brief Сохраняет несколько записей о частотности слов
     * @param documentId ID документа
     * @param wordFrequencies Карта: слово -> частота
     *
     * Оптимизированная пакетная вставка/обновление частотностей.
     * Сначала создаёт все слова (если не существуют), затем сохраняет частоты.
     */
    void saveWordFrequencies(Core::Domain::Model::Document::IdType documentId,
                             const std::map<std::string, int>& wordFrequencies) override;

    /**
     * @brief Ищет документы, содержащие все указанные слова
     * @param words Список слов для поиска
     * @return Список результатов поиска с релевантностью
     *
     * Выполняет сложный JOIN-запрос для поиска документов, содержащих ВСЕ слова.
     * Релевантность рассчитывается как сумма частот всех найденных слов.
     * Результаты сортируются по релевантности (по убыванию).
     */
    std::vector<Core::Domain::Model::SearchResult> search(const std::vector<std::string>& words) override;

  private:
    std::shared_ptr<DatabaseConnection> dbConnection_;

    /**
     * @brief Получает ID слова, создавая его при необходимости
     * @param text Текст слова
     * @return ID слова
     */
    Core::Domain::Model::Word::IdType getOrCreateWordId(const std::string& text);
};

} // namespace Infrastructure::Database
