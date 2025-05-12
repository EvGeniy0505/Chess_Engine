#include "engine.hpp"

namespace chess {

ComputerPlayer::ComputerPlayer(Color color, int difficulty)
    : moveGenerator(color, difficulty) {}

bool ComputerPlayer::makeMove(Board &board) {
  auto [fromX, fromY, toX, toY] = moveGenerator.generateBestMove(board);
  return board.makeMove(fromX, fromY, toX, toY);
}

} // namespace chess
