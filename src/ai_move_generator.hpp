#pragma once
#include "board/board.hpp"
#include <limits>
#include <vector>

namespace chess {

class AIMoveGenerator {
  public:
    AIMoveGenerator(Color color, int depth = 2);

    std::tuple<int, int, int, int> generateBestMove(Board &board);

  private:
    Color ai_color;
    int search_depth;

    int evaluatePosition(Board &board, int depth, bool maximizing, int alpha,
                         int beta);
    int calculateBoardScore(const Board &board);
    std::vector<std::tuple<int, int, int, int>>
    generateAllMoves(const Board &board, Color color);
};

} // namespace chess
