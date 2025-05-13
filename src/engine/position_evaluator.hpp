#pragma once
#include "board/board.hpp"
#include <map> // Добавлен include для std::map

namespace chess::engine {

class PositionEvaluator {
  public:
    virtual ~PositionEvaluator() = default;
    virtual int evaluate(const Board &board, Color color) = 0;
    static Color opposite_color(Color c);
};

class BasicEvaluator : public PositionEvaluator {
  public:
    BasicEvaluator() = default;
    int evaluate(const Board &board, Color color) override;
};

} // namespace chess::engine
