#include "engine/position_evaluator.hpp"

namespace chess::engine {

int PositionEvaluator::evaluate(const Board &board, Color color) {
    const bool endgame = is_endgame(board);
    return evaluate_material(board, color) + evaluate_positional(board, color) +
           evaluate_threats(board, color) +
           evaluate_pawn_structure(board, color) +
           evaluate_piece_mobility(board, color) +
           evaluate_king_safety(board, color);
}

bool PositionEvaluator::is_endgame(const Board &board) const {
    int queen_count = 0;
    int minor_pieces = 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.get_piece({x, y});
            if (piece.get_type() == PieceType::QUEEN)
                queen_count++;
            if (piece.get_type() == PieceType::KNIGHT ||
                piece.get_type() == PieceType::BISHOP)
                minor_pieces++;
        }
    }
    return queen_count == 0 || (queen_count == 1 && minor_pieces <= 2);
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

            int value = 0;
            switch (piece.get_type()) {
                case PieceType::PAWN:
                    value = PAWN_VALUE;
                    break;
                case PieceType::KNIGHT:
                    value = KNIGHT_VALUE;
                    break;
                case PieceType::BISHOP:
                    value = BISHOP_VALUE;
                    break;
                case PieceType::ROOK:
                    value = ROOK_VALUE;
                    break;
                case PieceType::QUEEN:
                    value = QUEEN_VALUE;
                    break;
                case PieceType::KING:
                    value = KING_VALUE;
                    break;
                default:
                    break;
            }

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
    const bool endgame = is_endgame(board);

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
                score += PieceSquareTables::get_value(piece.get_type(), pos,
                                                      color, endgame);
            }
        }
    }

    return score;
}

int PositionEvaluator::evaluate_threats(const Board &board, Color color) const {
    return board.is_check(opposite_color(color)) ? CHECK_BONUS : 0;
}

int PositionEvaluator::evaluate_pawn_structure(const Board &board,
                                               Color color) const {
    int score = 0;
    bool passed_pawns[8] = {false};

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Position pos{x, y};
            const auto &piece = board.get_piece(pos);
            if (piece.get_type() != PieceType::PAWN ||
                piece.get_color() != color)
                continue;

            bool is_passed = true;
            bool is_isolated = true;

            for (int dx = -1; dx <= 1; ++dx) {
                int file = x + dx;
                if (file < 0 || file > 7)
                    continue;

                if (dx != 0)
                    is_isolated = false;

                for (int rank = y + 1; rank < 8; ++rank) {
                    const auto &pawn = board.get_piece({file, rank});
                    if (pawn.get_type() == PieceType::PAWN &&
                        pawn.get_color() != color) {
                        is_passed = false;
                        break;
                    }
                }
                if (!is_passed)
                    break;
            }

            if (is_passed) {
                score +=
                    PASSED_PAWN_BONUS * (color == Color::WHITE ? (7 - y) : y);
                passed_pawns[x] = true;
            }
            if (is_isolated)
                score -= ISOLATED_PAWN_PENALTY;
        }
    }
    return score;
}

int PositionEvaluator::evaluate_piece_mobility(const Board &board,
                                               Color color) const {
    int mobility = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position pos{x, y};
            const auto &piece = board.get_piece(pos);
            if (piece.get_color() != color)
                continue;

            auto moves = board.get_legal_moves(pos);
            mobility += moves.size() * MOBILITY_BONUS;
        }
    }
    return mobility;
}

int PositionEvaluator::evaluate_king_safety(const Board &board,
                                            Color color) const {
    int safety = 0;
    Position king_pos = board.find_king(color);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int x = king_pos.first + dx;
            int y = king_pos.second + dy;
            if (x >= 0 && x < 8 && y >= 0 && y < 8) {
                const auto &piece = board.get_piece({x, y});
                if (piece.get_type() == PieceType::PAWN &&
                    piece.get_color() == color) {
                    safety += KING_SHIELD_BONUS;
                }
            }
        }
    }
    return safety;
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
