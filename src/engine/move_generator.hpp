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

    // MVV-LVA (Most Valuable Victim - Least Valuable Aggressor)
    int getMVVLVAscore(const Board &board, const Move &move) {
        const auto &victim = board.get_piece(move.to);
        const auto &aggressor = board.get_piece(move.from);

        static const std::map<PieceType, int> values = {
            {PieceType::PAWN, 100},   {PieceType::KNIGHT, 320},
            {PieceType::BISHOP, 330}, {PieceType::ROOK, 500},
            {PieceType::QUEEN, 900},  {PieceType::KING, 20000}};

        return values.at(victim.get_type()) - values.at(aggressor.get_type());
    }

    // Killer heuristic (сохраняем 2 лучших "убийцы" для каждого глубины)
    std::array<std::array<Move, 2>, 64> killer_moves_;

    // History heuristic (сколько раз ход приводил к beta-отсечению)
    std::array<std::array<int, 64>, 64> history_heuristic_;

    void sortMoves(std::vector<Move> &moves, const Board &board) {
        std::sort(
            moves.begin(), moves.end(), [&](const Move &a, const Move &b) {
                // 1. Сначала взятия (MVV-LVA)
                bool a_capture =
                    board.get_piece(a.to).get_type() != PieceType::NONE;
                bool b_capture =
                    board.get_piece(b.to).get_type() != PieceType::NONE;

                if (a_capture && !b_capture)
                    return true;
                if (!a_capture && b_capture)
                    return false;
                if (a_capture && b_capture) {
                    return getMVVLVAscore(board, a) > getMVVLVAscore(board, b);
                }

                // 2. Killer moves
                for (const auto &killer : killer_moves_[moves.size()]) {
                    if (a.from == killer.from && a.to == killer.to)
                        return true;
                    if (b.from == killer.from && b.to == killer.to)
                        return false;
                }

                // 3. History heuristic
                return history_heuristic_[a.from.first][a.from.second] >
                       history_heuristic_[b.from.first][b.from.second];
            });
    }
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
