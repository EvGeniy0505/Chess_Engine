#pragma once
#include "engine/move_generator.hpp"
#include "engine/opening_book.hpp"

namespace chess::engine {

class ComputerPlayer {
  public:
    ComputerPlayer(Color color, std::unique_ptr<MoveGenerator> generator);
    bool makeMove(Board &board);
    Move getLastMove() const;

    static std::unique_ptr<ComputerPlayer> create(Color color,
                                                  int difficulty = 2);
    Color color_;

  private:
    std::unique_ptr<MoveGenerator> generator_;
    OpeningBook openingBook_;
    Move lastMove_;
};

} // namespace chess::engine
