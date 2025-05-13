#include "engine/move_generator.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

namespace chess::engine {

std::vector<Move> MoveGenerator::generateAllMoves(const Board &board,
                                                  Color color) {
    std::vector<Move> moves;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position pos{x, y};
            const auto &piece = board.get_piece(pos);

            if (piece.get_color() == color) {
                auto legal_moves = board.get_legal_moves(pos);
                for (const auto &dest : legal_moves) {
                    Board temp = board;
                    temp.make_move(pos, dest);
                    if (!temp.is_check(color)) {
                        moves.push_back({pos, dest});
                    }
                }
            }
        }
    }
    return moves;
}

MinimaxGenerator::MinimaxGenerator(int depth,
                                   std::unique_ptr<PositionEvaluator> evaluator)
    : depth_(depth), evaluator_(std::move(evaluator)) {}

Move MinimaxGenerator::generateBestMove(Board &board, Color color) {
    std::vector<Move> best_moves;
    int best_score = std::numeric_limits<int>::min();

    auto moves = generateAllMoves(board, color);
    if (moves.empty())
        return {{0, 0}, {0, 0}};

    for (const auto &move : moves) {
        Board temp = board;
        temp.make_move(move.from, move.to);

        int score = minimax(temp, depth_ - 1, false, color,
                            std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max());

        if (score > best_score) {
            best_score = score;
            best_moves = {move};
        } else if (score == best_score) {
            best_moves.push_back(move);
        }
    }

    if (!best_moves.empty()) {
        std::random_device rd;
        return best_moves[rd() % best_moves.size()];
    }

    return moves[0];
}

int MinimaxGenerator::minimax(Board &board, int depth, bool maximizing,
                              Color eval_color, int alpha, int beta) {
    if (depth == 0 || board.is_checkmate(eval_color)) {
        return evaluator_->evaluate(board, eval_color);
    }

    Color current_player =
        maximizing ? eval_color : PositionEvaluator::opposite_color(eval_color);
    auto moves = generateAllMoves(board, current_player);

    if (maximizing) {
        int max_eval = std::numeric_limits<int>::min();
        for (const auto &move : moves) {
            Board temp = board;
            temp.make_move(move.from, move.to);

            int eval = minimax(temp, depth - 1, false, eval_color, alpha, beta);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return max_eval;
    } else {
        int min_eval = std::numeric_limits<int>::max();
        for (const auto &move : moves) {
            Board temp = board;
            temp.make_move(move.from, move.to);

            int eval = minimax(temp, depth - 1, true, eval_color, alpha, beta);
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return min_eval;
    }
}

} // namespace chess::engine
