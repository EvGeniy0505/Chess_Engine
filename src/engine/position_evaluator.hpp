#pragma once
#include "board/board.hpp"
#include "piece_square_tables.hpp"
#include <algorithm>

namespace chess::engine {

class PositionEvaluator {
public:
    virtual ~PositionEvaluator() = default;
    
    static Color opposite_color(Color c) {
        return c == Color::WHITE ? Color::BLACK : Color::WHITE;
    }
    
    int evaluate(const Board& board, Color color);

protected:
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    static constexpr int CENTER_BONUS = 10;
    static constexpr int DOUBLED_PAWN_PENALTY = 20;
    static constexpr int ISOLATED_PAWN_PENALTY = 30;
    static constexpr int PASSED_PAWN_BONUS = 50;
    static constexpr int MOBILITY_BONUS = 1;
    static constexpr int KING_SHIELD_BONUS = 20;
    static constexpr int CHECK_BONUS = 40;

    // Основные методы оценки
    bool is_endgame(const Board& board) const;
    int evaluate_material(const Board& board, Color color) const;
    int evaluate_positional(const Board& board, Color color) const;
    int evaluate_threats(const Board& board, Color color) const;
    int evaluate_pawn_structure(const Board& board, Color color) const;
    int evaluate_piece_mobility(const Board& board, Color color) const;
    int evaluate_king_safety(const Board& board, Color color) const;
    int doubled_pawns_penalty(const Board& board, Color color) const;
    int count_pawns_on_file(const Board& board, int file, Color color) const;
};

} // namespace chess::engine
