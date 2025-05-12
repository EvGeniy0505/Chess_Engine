#pragma once
#include "board.hpp"
#include <optional>
#include <random>

namespace chess {

class ComputerPlayer {
public:
    ComputerPlayer(Color color) : color_(color) {}
    bool makeMove(Board& board);
    Color getColor() const { return color_; } // Добавляем публичный метод для доступа к цвету

private:
    Color color_;
    std::random_device rd_;
    std::mt19937 gen_{rd_()};
};

} // namespace chess