#pragma once

#include <map>
#include <string>
#include <vector>

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
  Highlight
};

enum class PieceSet {
  UNICODE, // Стандартные Unicode символы
  LETTERS, // Буквенные обозначения (K, Q, R и т.д.)
  SIMPLE,  // Простые ASCII символы (аналогично LETTERS)
  FANTASY  // Альтернативные Unicode символы
};

struct Piece {
  PieceType type;
  Color color;
  std::string display;

  Piece() : type(PieceType::None), color(Color::White), display(".") {}
  Piece(PieceType t, Color c, const std::string &d)
      : type(t), color(c), display(d) {}

  std::string symbol(PieceSet set = PieceSet::UNICODE) const {
    if (type == PieceType::Highlight)
      return display;

    if (type == PieceType::None)
      return ".";

    static const std::map<PieceSet, std::vector<std::vector<std::string>>>
        symbols = {{PieceSet::UNICODE,
                    {
                        {"♟", "♞", "♝", "♜", "♛", "♚"}, // Белые
                        {"♙", "♘", "♗", "♖", "♕", "♔"}  // Чёрные
                    }},
                   {PieceSet::LETTERS,
                    {
                        {"P", "N", "B", "R", "Q", "K"}, // Белые
                        {"p", "n", "b", "r", "q", "k"}  // Чёрные
                    }}};

    int idx = static_cast<int>(type) - 1;
    return symbols.at(set)[color == Color::White ? 0 : 1][idx];
  }
};

} // namespace chess
