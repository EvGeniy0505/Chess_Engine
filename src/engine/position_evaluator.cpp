#include "engine/position_evaluator.hpp"
#include <map> // Добавлен include

namespace chess::engine {

Color PositionEvaluator::opposite_color(Color c) {
    return c == Color::WHITE ? Color::BLACK : Color::WHITE;
}

int BasicEvaluator::evaluate(const Board &board, Color color) {
    int score = 0;
    // Исправлено: добавлен std:: перед map
    const std::map<PieceType, int> values = {
        {PieceType::PAWN, 100},   {PieceType::KNIGHT, 320},
        {PieceType::BISHOP, 330}, {PieceType::ROOK, 500},
        {PieceType::QUEEN, 900},  {PieceType::KING, 20000}};

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.get_piece({x, y});
            if (piece.get_type() != PieceType::NONE) {
                int value = values.at(piece.get_type());
                score += (piece.get_color() == color) ? value : -value;
            }
        }
    }

    if (board.is_check(color))
        score -= 50;
    if (board.is_check(opposite_color(color)))
        score += 50;

    return score;
}

} // namespace chess::engine
