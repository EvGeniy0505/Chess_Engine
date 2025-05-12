#pragma once

namespace chess {

enum class PieceType {
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    HIGHLIGHT
};

enum class PieceSet { UNICODE, LETTERS };

} // namespace chess
