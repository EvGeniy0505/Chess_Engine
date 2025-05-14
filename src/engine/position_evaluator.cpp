#include "engine/position_evaluator.hpp"

namespace chess::engine {

Color PositionEvaluator::opposite_color(Color c) {
    return c == Color::WHITE ? Color::BLACK : Color::WHITE;
}

int PositionEvaluator::get_material_value(PieceType type) const {
    switch (type) {
        case PieceType::PAWN:
            return PAWN_VALUE;
        case PieceType::KNIGHT:
            return KNIGHT_VALUE;
        case PieceType::BISHOP:
            return BISHOP_VALUE;
        case PieceType::ROOK:
            return ROOK_VALUE;
        case PieceType::QUEEN:
            return QUEEN_VALUE;
        case PieceType::KING:
            return KING_VALUE;
        default:
            return 0;
    }
}

int PositionEvaluator::evaluate(const Board &board, Color color) {
    int material = evaluate_material(board, color);
    int positional = evaluate_positional(board, color);
    int threats = evaluate_threats(board, color);

    int score = material + positional + threats;
    return score;
}

int PositionEvaluator::evaluate_material(const Board &board,
                                         Color color) const {
    int white_material = 0;
    int black_material = 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position pos{x, y};
            const auto &piece = board.get_piece(pos);
            if (piece.get_type() == PieceType::NONE)
                continue;

            int value = get_material_value(piece.get_type());
            if (piece.get_color() == Color::WHITE) {
                white_material += value;
            } else {
                black_material += value;
            }
        }
    }

    return (color == Color::WHITE) ? (white_material - black_material)
                                   : (black_material - white_material);
}

int PositionEvaluator::evaluate_positional(const Board &board,
                                           Color color) const {
    int score = 0;

    constexpr Position center[] = {{3, 3}, {4, 3}, {3, 4}, {4, 4}};
    for (auto pos : center) {
        const auto &piece = board.get_piece(pos);
        if (piece.get_type() != PieceType::NONE && piece.get_color() == color) {
            score += CENTER_BONUS;
        }
    }

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position pos{x, y};
            const auto &piece = board.get_piece(pos);
            if (piece.get_type() != PieceType::NONE &&
                piece.get_color() == color) {
                score +=
                    PieceSquareTables::get_value(piece.get_type(), pos, color);
            }
        }
    }

    score -= doubled_pawns_penalty(board, color);

    return score;
}

int PositionEvaluator::evaluate_threats(const Board &board, Color color) const {
    int score = 0;

    if (board.is_check(opposite_color(color))) {
        score += CHECK_BONUS;
    }

    return score;
}

int PositionEvaluator::doubled_pawns_penalty(const Board &board,
                                             Color color) const {
    int penalty = 0;
    for (int file = 0; file < 8; ++file) {
        int pawns = count_pawns_on_file(board, file, color);
        if (pawns > 1) {
            penalty += DOUBLED_PAWN_PENALTY * (pawns - 1);
        }
    }
    return penalty;
}

int PositionEvaluator::count_pawns_on_file(const Board &board, int file,
                                           Color color) const {
    int count = 0;
    for (int rank = 0; rank < 8; ++rank) {
        auto piece = board.get_piece({file, rank});
        if (piece.get_type() == PieceType::PAWN && piece.get_color() == color) {
            count++;
        }
    }
    return count;
}
} // namespace chess::engine
