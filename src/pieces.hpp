#pragma once

#include <cstdint>
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
    Highlight
};

struct Piece {
    PieceType type;
    Color color;
    char display;
    
    char symbol() const {
        if (type == PieceType::Highlight) return display;
        
        switch (type) {
            case PieceType::King:   return color == Color::White ? 'K' : 'k';
            case PieceType::Queen:  return color == Color::White ? 'Q' : 'q';
            case PieceType::Rook:   return color == Color::White ? 'R' : 'r';
            case PieceType::Bishop: return color == Color::White ? 'B' : 'b';
            case PieceType::Knight: return color == Color::White ? 'N' : 'n';
            case PieceType::Pawn:   return color == Color::White ? 'P' : 'p';
            default: return '.';
        }
    }
};

} // namespace chess
