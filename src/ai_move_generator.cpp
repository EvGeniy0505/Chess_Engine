#include "ai_move_generator.hpp"
#include <algorithm>
#include <random>

namespace chess {

// Конструктор
AIMoveGenerator::AIMoveGenerator(Color color, int depth)
    : ai_color(color), search_depth(depth) {}

// Основной метод генерации хода
std::tuple<int, int, int, int> AIMoveGenerator::generateBestMove(Board &board) {
  int bestScore = std::numeric_limits<int>::min();
  std::vector<std::tuple<int, int, int, int>> bestMoves;

  auto possibleMoves = generateAllMoves(board, ai_color);

  for (const auto &move : possibleMoves) {
    auto [fromX, fromY, toX, toY] = move;
    Board tempBoard = board;
    tempBoard.makeMove(fromX, fromY, toX, toY);

    int moveScore = evaluatePosition(tempBoard, search_depth - 1, false,
                                     std::numeric_limits<int>::min(),
                                     std::numeric_limits<int>::max());

    if (moveScore > bestScore) {
      bestScore = moveScore;
      bestMoves = {move};
    } else if (moveScore == bestScore) {
      bestMoves.push_back(move);
    }
  }

  if (!bestMoves.empty()) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(gen)];
  }

  return possibleMoves.empty() ? std::make_tuple(0, 0, 0, 0) : possibleMoves[0];
}

int AIMoveGenerator::evaluatePosition(Board &board, int depth, bool maximizing,
                                      int alpha, int beta) {
  if (depth == 0 || board.isCheckmate(ai_color)) {
    return calculateBoardScore(board);
  }

  if (maximizing) {
    int maxEval = std::numeric_limits<int>::min();
    auto moves = generateAllMoves(board, ai_color);

    for (const auto &move : moves) {
      auto [fromX, fromY, toX, toY] = move;
      Board tempBoard = board;
      tempBoard.makeMove(fromX, fromY, toX, toY);

      int eval = evaluatePosition(tempBoard, depth - 1, false, alpha, beta);
      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
        break;
    }
    return maxEval;
  } else {
    int minEval = std::numeric_limits<int>::max();
    Color opponent = (ai_color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    auto moves = generateAllMoves(board, opponent);

    for (const auto &move : moves) {
      auto [fromX, fromY, toX, toY] = move;
      Board tempBoard = board;
      tempBoard.makeMove(fromX, fromY, toX, toY);

      int eval = evaluatePosition(tempBoard, depth - 1, true, alpha, beta);
      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha)
        break;
    }
    return minEval;
  }
}

int AIMoveGenerator::calculateBoardScore(const Board &board) {
  int score = 0;

  // Материальная оценка
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      const auto &piece = board.getPiece(x, y);
      int pieceValue = 0;

      switch (piece.type) {
      case PieceType::PAWN:
        pieceValue = 100;
        break;
      case PieceType::KNIGHT:
        pieceValue = 320;
        break;
      case PieceType::BISHOP:
        pieceValue = 330;
        break;
      case PieceType::ROOK:
        pieceValue = 500;
        break;
      case PieceType::QUEEN:
        pieceValue = 900;
        break;
      case PieceType::KING:
        pieceValue = 20000;
        break;
      default:
        break;
      }

      score += (piece.color == ai_color) ? pieceValue : -pieceValue;
    }
  }

  // Простая позиционная оценка (можно расширить)
  if (board.isCheck(ai_color)) {
    score -= 50;
  }
  if (board.isCheck(ai_color == Color::WHITE ? Color::BLACK : Color::WHITE)) {
    score += 50;
  }

  return score;
}

std::vector<std::tuple<int, int, int, int>>
AIMoveGenerator::generateAllMoves(const Board &board, Color color) {
  std::vector<std::tuple<int, int, int, int>> moves;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      if (board.getPiece(x, y).color == color) {
        auto pieceMoves = board.getPossibleMoves(x, y);
        for (const auto &[toX, toY] : pieceMoves) {
          moves.emplace_back(x, y, toX, toY);
        }
      }
    }
  }

  return moves;
}

} // namespace chess
