#pragma once
#include "ai_move_generator.hpp"

namespace chess {

class ComputerPlayer {
public:
  ComputerPlayer(Color color, int difficulty = 2);
  bool makeMove(Board &board);

private:
  AIMoveGenerator moveGenerator;
};

} // namespace chess
