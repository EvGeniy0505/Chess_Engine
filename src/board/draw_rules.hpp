#pragma once
#include "board/board.hpp"
#include "pieces/piece.hpp"

namespace chess {

class DrawRules {
  public:
    static bool is_draw(const Board &board);
    static bool is_stalemate(const Board &board, Color player);
    static bool insufficient_material(const Board &board);
    static bool is_repetition(const Board &board);
    static bool is_fifty_move_rule(const Board &board);

  private:
    static bool has_insufficient_material(Color color, const Board &board);
    static bool is_bishop_vs_bishop(const Board &board);
};

} // namespace chess
