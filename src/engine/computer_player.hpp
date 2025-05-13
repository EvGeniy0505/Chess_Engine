#pragma once
#include "engine/move_generator.hpp"

namespace chess::engine {

class ComputerPlayer {
  public:
    ComputerPlayer(Color color, std::unique_ptr<MoveGenerator> generator);
    bool makeMove(Board &board);

    static std::unique_ptr<ComputerPlayer> create(Color color,
                                                  int difficulty = 2);

  private:
    std::unique_ptr<MoveGenerator> generator_;
    Color color_;
};

} // namespace chess::engine
