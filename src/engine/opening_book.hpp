#pragma once

#include "board/board.hpp"
#include "engine/move_generator.hpp"
#include <map>
#include <optional>
#include <string>

namespace chess::engine {

class OpeningBook {
public:
    // Конструктор загружает дебютную базу из файла
    explicit OpeningBook(const std::string& filename);

    // Получить ход из дебютной книги для позиции (если доступен)
    std::optional<Move> getOpeningMove(const Board &board, Color color) const;

private:
    // Дебютная книга: сопоставление FEN → список ходов с частотами
    std::map<std::string, std::vector<std::pair<Move, int>>> book_;

    // Конвертация позиции в FEN без счетчиков ходов (для ключа)
    std::string boardToFEN(const Board &board, Color color) const;

    // Удаление счетчиков из FEN (поле 5 и 6)
    static std::string removeMoveCounters(const std::string& fen);

    // Парсинг хода из формата "e2e4" в Move
    static std::optional<Move> parseMove(const std::string& moveStr);

    static std::string trim(const std::string& s);

};

} // namespace chess::engine
