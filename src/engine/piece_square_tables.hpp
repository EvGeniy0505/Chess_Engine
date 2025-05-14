#pragma once
#include "board/board.hpp"
#include <array>

namespace chess::engine {
using Position = std::pair<int, int>;

struct PieceSquareTables {
    // Все оригинальные таблицы + новые для эндшпиля
    static constexpr std::array<std::array<int, 8>, 8> PAWN = {
        {{0, 0, 0, 0, 0, 0, 0, 0},
         {50, 50, 50, 50, 50, 50, 50, 50},
         {10, 10, 20, 30, 30, 20, 10, 10},
         {5, 5, 10, 25, 25, 10, 5, 5},
         {0, 0, 0, 20, 20, 0, 0, 0},
         {5, -5, -10, 0, 0, -10, -5, 5},
         {5, 10, 10, -20, -20, 10, 10, 5},
         {0, 0, 0, 0, 0, 0, 0, 0}}};

    static constexpr std::array<std::array<int, 8>, 8> KNIGHT = {
        {{-50, -40, -30, -30, -30, -30, -40, -50},
         {-40, -20, 0, 5, 5, 0, -20, -40},
         {-30, 5, 10, 15, 15, 10, 5, -30},
         {-30, 0, 15, 20, 20, 15, 0, -30},
         {-30, 5, 15, 20, 20, 15, 5, -30},
         {-30, 0, 10, 15, 15, 10, 0, -30},
         {-40, -20, 0, 0, 0, 0, -20, -40},
         {-50, -40, -30, -30, -30, -30, -40, -50}}};

    static constexpr std::array<std::array<int, 8>, 8> BISHOP = {
        {{-20, -10, -10, -10, -10, -10, -10, -20},
         {-10, 5, 0, 0, 0, 0, 5, -10},
         {-10, 10, 10, 10, 10, 10, 10, -10},
         {-10, 0, 10, 10, 10, 10, 0, -10},
         {-10, 5, 5, 10, 10, 5, 5, -10},
         {-10, 0, 5, 10, 10, 5, 0, -10},
         {-10, 0, 0, 0, 0, 0, 0, -10},
         {-20, -10, -10, -10, -10, -10, -10, -20}}};

    static constexpr std::array<std::array<int, 8>, 8> ROOK = {
        {{0, 0, 0, 5, 5, 0, 0, 0},
         {-5, 0, 0, 0, 0, 0, 0, -5},
         {-5, 0, 0, 0, 0, 0, 0, -5},
         {-5, 0, 0, 0, 0, 0, 0, -5},
         {-5, 0, 0, 0, 0, 0, 0, -5},
         {-5, 0, 0, 0, 0, 0, 0, -5},
         {5, 10, 10, 10, 10, 10, 10, 5},
         {0, 0, 0, 0, 0, 0, 0, 0}}};

    static constexpr std::array<std::array<int, 8>, 8> QUEEN = {
        {{-20, -10, -10, -5, -5, -10, -10, -20},
         {-10, 0, 5, 0, 0, 0, 0, -10},
         {-10, 5, 5, 5, 5, 5, 0, -10},
         {0, 0, 5, 5, 5, 5, 0, -5},
         {-5, 0, 5, 5, 5, 5, 0, -5},
         {-10, 0, 5, 5, 5, 5, 0, -10},
         {-10, 0, 0, 0, 0, 0, 0, -10},
         {-20, -10, -10, -5, -5, -10, -10, -20}}};

    static constexpr std::array<std::array<int, 8>, 8> KING_MIDDLEGAME = {
        {{20, 30, 10, 0, 0, 10, 30, 20},
         {20, 20, 0, 0, 0, 0, 20, 20},
         {-10, -20, -20, -20, -20, -20, -20, -10},
         {-20, -30, -30, -40, -40, -30, -30, -20},
         {-30, -40, -40, -50, -50, -40, -40, -30},
         {-30, -40, -40, -50, -50, -40, -40, -30},
         {-30, -40, -40, -50, -50, -40, -40, -30},
         {-30, -40, -40, -50, -50, -40, -40, -30}}};

    static constexpr std::array<std::array<int, 8>, 8> KING_ENDGAME = {
        {{-50, -40, -30, -20, -20, -30, -40, -50},
         {-30, -20, -10, 0, 0, -10, -20, -30},
         {-30, -10, 20, 30, 30, 20, -10, -30},
         {-30, -10, 30, 40, 40, 30, -10, -30},
         {-30, -10, 30, 40, 40, 30, -10, -30},
         {-30, -10, 20, 30, 30, 20, -10, -30},
         {-30, -30, 0, 0, 0, 0, -30, -30},
         {-50, -30, -30, -30, -30, -30, -30, -50}}};

    static int get_value(PieceType type, Position pos, Color color, bool endgame) {
        int y = (color == Color::WHITE) ? pos.second : 7 - pos.second;
        int x = pos.first;

        switch (type) {
            case PieceType::PAWN: return PAWN[y][x];
            case PieceType::KNIGHT: return KNIGHT[y][x];
            case PieceType::BISHOP: return BISHOP[y][x];
            case PieceType::ROOK: return ROOK[y][x];
            case PieceType::QUEEN: return QUEEN[y][x];
            case PieceType::KING: return endgame ? KING_ENDGAME[y][x] : KING_MIDDLEGAME[y][x];
            default: return 0;
        }
    }
};
} // namespace chess::engine
