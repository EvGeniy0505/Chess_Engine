#pragma once
#include "board/board.hpp"
#include "piece_square_tables.hpp"
#include <algorithm>

namespace chess::engine {

class PositionEvaluator {
  public:
    virtual ~PositionEvaluator() = default;
    int evaluate(const Board &board, Color color);
    static Color opposite_color(Color c);

  protected:
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    static constexpr int CENTER_BONUS = 10;
    static constexpr int DOUBLED_PAWN_PENALTY = 30;
    static constexpr int CHECK_BONUS = 40;

    int evaluate_material(const Board &board, Color color) const;
    int evaluate_positional(const Board &board, Color color) const;
    int evaluate_threats(const Board &board, Color color) const;
    int doubled_pawns_penalty(const Board &board, Color color) const;
    int count_pawns_on_file(const Board &board, int file, Color color) const;
    int get_material_value(PieceType type) const;
};
} // namespace chess::engine
