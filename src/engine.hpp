#pragma once
#include "board.hpp"
#include <optional>
#include <random>

namespace chess {

class ComputerPlayer {
public:
  ComputerPlayer(Color color) : color_(color) {}

  std::optional<std::tuple<int, int, int, int>>
  generateMove(const Board &board);
  bool makeMove(Board &board);

private:
  Color color_;
  std::random_device rd_;
  std::mt19937 gen_{rd_()};
};

} // namespace chess
