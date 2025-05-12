#pragma once
#include "board/board.hpp"

namespace chess {
class BoardInitializer {
  public:
    static void setup_initial_position(Board &board);
};
} // namespace chess
