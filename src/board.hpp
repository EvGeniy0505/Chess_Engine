#pragma once

#include "pieces.hpp"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace chess {

class Board {
public:
  // Добавляем конструктор с параметром PieceSet и устанавливаем значение по
  // умолчанию
  Board(PieceSet set = PieceSet::UNICODE);

  // Добавляем методы для работы с набором фигур
  void setPieceSet(PieceSet set);
  PieceSet getPieceSet() const;

  void print(bool showHighlights = false) const;
  bool makeMove(int fromX, int fromY, int toX, int toY,
                PieceType promotion = PieceType::None);
  bool isCheck(Color player) const;
  bool isCheckmate(Color player);
  std::vector<std::pair<int, int>> getPossibleMoves(int x, int y) const;
  void highlightMoves(const std::vector<std::pair<int, int>> &moves);
  void clearHighlights();

private:
  std::array<std::array<Piece, 8>, 8> grid_;
  Color current_player_ = Color::White;
  PieceSet piece_set_ =
      PieceSet::UNICODE; // Добавляем поле для хранения текущего набора

  bool white_king_moved_ = false;
  bool white_kingside_rook_moved_ = false;
  bool white_queenside_rook_moved_ = false;
  bool black_king_moved_ = false;
  bool black_kingside_rook_moved_ = false;
  bool black_queenside_rook_moved_ = false;

  bool isInBounds(int x, int y) const;
  bool isEmpty(int x, int y) const;
  bool isEnemy(int x, int y, Color allyColor) const;
  bool isSquareAttacked(int x, int y, Color byColor) const;
  std::vector<std::pair<int, int>> generatePseudoLegalMoves(int x, int y) const;
  bool castle(int kingX, int kingY, int toX, int toY);
  bool canCastleKingside(Color player) const;
  bool canCastleQueenside(Color player) const;
  void initializeBoard();
  void updateCastlingState(int x, int y);
  std::string
  getPieceSymbol(PieceType type,
                 Color color) const; // Можно оставить или удалить, если будем
                                     // использовать symbol() из Piece
};

} // namespace chess
