#pragma once

#include "pieces/piece_color.hpp"
#include "pieces/piece_symbols.hpp"
#include "pieces/piece_types.hpp"
#include <string>

namespace chess {

class Piece {
  public:
    Piece() = default;
    Piece(PieceType type, Color color, CellColor cellColor = CellColor::WHITE)
        : type_(type), color_(color), cellColor_(cellColor) {}

    // Геттеры
    PieceType get_type() const { return type_; }
    Color get_color() const { return color_; }
    CellColor get_cell_color() const { return cellColor_; }

    // Сеттеры
    void set_type(PieceType type) { type_ = type; }
    void set_color(Color color) { color_ = color; }
    void set_cell_color(CellColor color) { cellColor_ = color; }

    std::string getSymbol(PieceSet set = PieceSet::UNICODE) const {
        return PieceSymbols::get(type_, color_, set);
    }

    char to_char() const {
        switch (type_) {
            case PieceType::PAWN:
                return (color_ == Color::WHITE) ? 'P' : 'p';
            case PieceType::KNIGHT:
                return (color_ == Color::WHITE) ? 'N' : 'n';
            case PieceType::BISHOP:
                return (color_ == Color::WHITE) ? 'B' : 'b';
            case PieceType::ROOK:
                return (color_ == Color::WHITE) ? 'R' : 'r';
            case PieceType::QUEEN:
                return (color_ == Color::WHITE) ? 'Q' : 'q';
            case PieceType::KING:
                return (color_ == Color::WHITE) ? 'K' : 'k';
            case PieceType::NONE:
                return '.';
        }
        return '?'; // На случай ошибки
    }

    std::string getColoredSymbol(PieceSet set = PieceSet::UNICODE) const {
        std::string symbol = PieceSymbols::get(type_, color_, set);
        const auto &codes = getColorCodes(cellColor_);
        const char *fg = (color_ == Color::WHITE) ? codes.foreground_white
                                                  : codes.foreground_black;

        std::string result;
        result.reserve(32);
        result.append("\033[1;")
            .append(fg)
            .append(";")
            .append(codes.background)
            .append("m ")
            .append(symbol)
            .append(" \033[0m");
        return result;
    }

  private:
    PieceType type_ = PieceType::NONE;
    Color color_ = Color::WHITE;
    CellColor cellColor_ = CellColor::WHITE;
};

} // namespace chess
