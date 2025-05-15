#pragma once
#include "engine/move_generator.hpp"

namespace chess::engine {

class ComputerPlayer {
  public:
    ComputerPlayer(Color color, std::unique_ptr<MoveGenerator> generator);
    bool makeMove(Board &board);
    Move getLastMove() const; // Добавьте этот метод

    static std::unique_ptr<ComputerPlayer> create(Color color,
                                                  int difficulty = 2);
  Color color_;

  private:
    std::unique_ptr<MoveGenerator> generator_;
    Move lastMove_; // Добавьте поле для хранения последнего хода
};

} // namespace chess::engine
