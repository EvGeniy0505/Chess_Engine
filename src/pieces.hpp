#pragma once
#include <string>

namespace chess {

enum class Color { White, Black };

enum class PieceType {
  None,
  Pawn,
  Knight,
  Bishop,
  Rook,
  Queen,
  King,
  Highlight // Добавляем для подсветки ходов
};

struct Piece {
  PieceType type;
  Color color;
  std::string display; // Символ для отображения

  Piece() : type(PieceType::None), color(Color::White), display(".") {}
  Piece(PieceType t, Color c, const std::string &d)
      : type(t), color(c), display(d) {}

  std::string symbol() const { // Возвращаем строку
    if (type == PieceType::Highlight)
      return display;

    static const std::string symbols[2][6] = {
        // Массив строк
        {"♟", "♞", "♝", "♜", "♛", "♚"}, // Чёрные
        {"♙", "♘", "♗", "♖", "♕", "♔"}  // Белые
    };

    if (type == PieceType::None)
      return ".";
    int idx = static_cast<int>(type) - 1;
    return symbols[color == Color::White ? 0 : 1][idx];
  }
};
} // namespace chess
