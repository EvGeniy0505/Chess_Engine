#pragma once

#include <map>
#include <string>
#include <vector>

namespace chess {

enum class Color { White, Black };

enum class CellColor { WHITE, BLACK, HIGHLIGHT_WHITE, HIGHLIGHT_BLACK };

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
  SIMPLE,  // Простые ASCII символы 
  FANTASY  // Альтернативные Unicode символы
};

struct Piece {
  PieceType type;
  Color color;
  std::string display;
  CellColor cell_color = CellColor::WHITE; // Добавляем цвет клетки

  Piece() : type(PieceType::None), color(Color::White), display(".") {}
  Piece(PieceType t, Color c, const std::string &d)
      : type(t), color(c), display(d) {}

  std::string getSymbol(PieceSet set = PieceSet::UNICODE) const {
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

  std::string getColoredSymbol(PieceSet set = PieceSet::UNICODE) const {
    std::string symbol = getSymbol(set);

    std::string bg_code;
    std::string fg_code;

    switch (cell_color) {
    case CellColor::WHITE:
      bg_code = "48;5;237"; // Тёмно-серый
      fg_code = (color == Color::White) ? "38;5;15" : "38;5;250";
      break;

    case CellColor::BLACK:
      bg_code = "48;5;236"; // Почти тёмный
      fg_code = (color == Color::White) ? "38;5;15" : "38;5;245";
      break;

    case CellColor::HIGHLIGHT_WHITE:
    case CellColor::HIGHLIGHT_BLACK:
      bg_code = "48;5;238"; // Тёмно-серый с подсветкой
      fg_code = (color == Color::White) ? "38;5;15" : "38;5;250";
      break;
    }

    return "\033[1;" + fg_code + ";" + bg_code + "m " + symbol + " \033[0m";
  }
};
} // namespace chess
