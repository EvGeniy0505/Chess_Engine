#pragma once
#include "board/board.hpp"

namespace chess {
class CastlingManager {
  public:
    static bool try_perform_castle(Board &board, std::pair<int, int> king_from,
                                   std::pair<int, int> king_to);

    static void update_castling_rights(Board &board,
                                       std::pair<int, int> moved_from);

    static bool can_castle_kingside(const Board &board, Color color);

    static bool can_castle_queenside(const Board &board, Color color);
};
} // namespace chess
