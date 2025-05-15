#pragma once
#include "board/board.hpp"

namespace chess {
class CheckValidator {
  public:
    static bool is_check(const Board &board, Color player);

    static bool is_checkmate(Board &board, Color player);
    
    static bool is_stalemate(Board &board, Color player);

    static bool is_attacked(const Board &board, std::pair<int, int> square,
                            Color by_color);
};
} // namespace chess
