#pragma once

#include "pieces/piece_color.hpp"
#include "pieces/piece_types.hpp"
#include <array>

namespace chess {

class PieceSymbols {
  public:
    static const char *get(PieceType type, Color color, PieceSet set) {
        if (type == PieceType::NONE)
            return ".";
        if (type == PieceType::HIGHLIGHT)
            return " ";

        const auto &setSymbols =
            (set == PieceSet::UNICODE) ? unicodeSymbols : letterSymbols;
        return setSymbols[color == Color::WHITE ? 0 : 1]
                         [static_cast<int>(type) - 1];
    }

  private:
    static constexpr std::array<std::array<const char *, 6>, 2> unicodeSymbols =
        {{{"♟", "♞", "♝", "♜", "♛", "♚"}, {"♙", "♘", "♗", "♖", "♕", "♔"}}};

    static constexpr std::array<std::array<const char *, 6>, 2> letterSymbols =
        {{{"P", "N", "B", "R", "Q", "K"}, {"p", "n", "b", "r", "q", "k"}}};
};

} // namespace chess
