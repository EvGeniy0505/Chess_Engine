#pragma once

#include "pieces/color.hpp"
#include "pieces/pieces_types.hpp"
#include "pieces/symbols.hpp"
#include <string>

namespace chess {

class Piece {
  public:
    Piece() = default;
    Piece(PieceType type, Color color, CellColor cellColor = CellColor::WHITE)
        : type_(type), color_(color), cellColor_(cellColor) {}

    // Геттеры
    PieceType getType() const { return type_; }
    Color getColor() const { return color_; }
    CellColor getCellColor() const { return cellColor_; }

    // Сеттеры
    void setType(PieceType type) { type_ = type; }
    void setColor(Color color) { color_ = color; }
    void setCellColor(CellColor color) { cellColor_ = color; }

    std::string getSymbol(PieceSet set = PieceSet::UNICODE) const {
        return PieceSymbols::get(type_, color_, set);
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
