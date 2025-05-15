#include "engine/move_generator.hpp"
#include "engine/engine_logger.hpp"
#include <algorithm>
#include <chrono>
#include <limits>
#include <random>

namespace chess::engine {

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

// Время (в мс) в зависимости от сложности (difficulty)
constexpr std::array<int, 5> TIME_LIMITS = {
    500,  // difficulty=1 (новичок)
    1000, // difficulty=2 (любитель)
    2000, // difficulty=3 (профессионал)
    5000, // difficulty=4 (мастер)
    10000 // difficulty=5 (гроссмейстер)
};

std::vector<Move> MoveGenerator::generateAllMoves(const Board &board,
                                                  Color color) {
    std::vector<Move> moves;
    std::vector<Move> captures;
    std::vector<Move> nonCaptures;

    // Разделяем ходы на взятия и обычные
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position pos{x, y};
            const auto &piece = board.get_piece(pos);

            if (piece.get_color() == color) {
                auto legal_moves = board.get_legal_moves(pos);
                for (const auto &dest : legal_moves) {
                    if (board.get_piece(dest).get_type() != PieceType::NONE) {
                        captures.push_back({pos, dest});
                    } else {
                        nonCaptures.push_back({pos, dest});
                    }
                }
            }
        }
    }

    // Сортируем взятия по MVV-LVA
    std::sort(captures.begin(), captures.end(),
              [&board](const Move &a, const Move &b) {
                  int a_value =
                      static_cast<int>(board.get_piece(a.to).get_type());
                  int b_value =
                      static_cast<int>(board.get_piece(b.to).get_type());
                  return a_value > b_value;
              });

    moves.insert(moves.end(), captures.begin(), captures.end());
    moves.insert(moves.end(), nonCaptures.begin(), nonCaptures.end());
    return moves;
}

MinimaxGenerator::MinimaxGenerator(int depth,
                                   std::unique_ptr<PositionEvaluator> evaluator)
    : depth_(depth), evaluator_(std::move(evaluator)) {}

Move MinimaxGenerator::generateBestMove(Board &board, Color color) {
    DebugLogger logger(color);
    std::vector<std::pair<Move, int>> scored_moves;
    auto moves = generateAllMoves(board, color);
    
    if (moves.empty()) return {{0, 0}, {0, 0}};

    Move best_move = moves[0];
    int best_score = std::numeric_limits<int>::min();
    
    // Итеративное углубление
    for (int current_depth = 1; current_depth <= depth_; ++current_depth) {
        for (const auto &move : moves) {
            Board temp = board;
            temp.make_move(move.from, move.to);
            
            int score = minimax(temp, current_depth - 1, false, color,
                               std::numeric_limits<int>::min(),
                               std::numeric_limits<int>::max());
            
            logger.log_move(move.from, move.to, score);
            
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }
    }

    return best_move;
}

int MinimaxGenerator::minimax(Board &board, int depth, bool maximizing,
                              Color eval_color, int alpha, int beta) {
    if (depth == 0 || board.is_checkmate(eval_color) || board.is_draw()) {
        return evaluator_->evaluate(board, eval_color);
    }

    Color current_player =
        maximizing ? eval_color : PositionEvaluator::opposite_color(eval_color);
    auto moves = generateAllMoves(board, current_player);

    // Оптимизация: сортировка ходов (подробнее в п.3)
    sortMoves(moves, board);

    if (maximizing) {
        int max_eval = std::numeric_limits<int>::min();
        for (const auto &move : moves) {
            Board temp = board;
            temp.make_move(move.from, move.to);
            int eval = minimax(temp, depth - 1, false, eval_color, alpha, beta);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break; // Beta-отсечение
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
                break; // Alpha-отсечение
        }
        return min_eval;
    }
}

} // namespace chess::engine
