#pragma once
#include "board/board.hpp"
#include "engine/position_evaluator.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace chess::engine {

using Position = std::pair<int, int>;

struct Move {
    Position from;
    Position to;
};

class MoveGenerator {
  public:
    virtual ~MoveGenerator() = default;
    virtual Move generateBestMove(Board &board, Color color) = 0;
    std::vector<Move> generateAllMoves(const Board &board, Color color);
};

class MinimaxGenerator : public MoveGenerator {
  public:
    MinimaxGenerator(int depth, std::unique_ptr<PositionEvaluator> evaluator);
    Move generateBestMove(Board &board, Color color) override;

  private:
    int depth_;
    std::unique_ptr<PositionEvaluator> evaluator_;

    int minimax(Board &board, int depth, bool maximizing, Color eval_color,
                int alpha, int beta);
};

} // namespace chess::engine
