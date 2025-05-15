#pragma once
#include "board/board.hpp"
#include <map>
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

    int getMVVLVAscore(const Board &board, const Move &move) {
        const auto &victim = board.get_piece(move.to);
        const auto &aggressor = board.get_piece(move.from);

        static const std::map<PieceType, int> values = {
            {PieceType::PAWN, 100},   {PieceType::KNIGHT, 320},
            {PieceType::BISHOP, 330}, {PieceType::ROOK, 500},
            {PieceType::QUEEN, 900},  {PieceType::KING, 20000}};

        return values.at(victim.get_type()) - values.at(aggressor.get_type());
    }

    void sortMoves(std::vector<Move> &moves, const Board &board) {
        std::sort(
            moves.begin(), moves.end(), [&](const Move &a, const Move &b) {
                bool a_capture = board.get_piece(a.to).get_type() != PieceType::NONE;
                bool b_capture = board.get_piece(b.to).get_type() != PieceType::NONE;

                if (a_capture != b_capture) 
                    return a_capture > b_capture;
                if (a_capture && b_capture) 
                    return getMVVLVAscore(board, a) > getMVVLVAscore(board, b);
                
                return false;
            });
    }

protected:
    std::array<std::array<Move, 2>, 64> killer_moves_;
    std::array<std::array<int, 64>, 64> history_heuristic_;
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
