#pragma once
#include "board/board.hpp"
#include <vector>

namespace chess {
class MoveGenerator {
  public:
    static std::vector<std::pair<int, int>>
    generate_pseudo_legal_moves(const Board &board,
                                std::pair<int, int> position);

    static std::vector<std::pair<int, int>>
    get_legal_moves(const Board &board, std::pair<int, int> position);
};
} // namespace chess
