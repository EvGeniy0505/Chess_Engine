#include "board/draw_rules.hpp"
#include "board/check.hpp"
#include "board/move_generation.hpp"
#include <unordered_set>

namespace chess {

bool DrawRules::is_draw(const Board &board) {
    return is_stalemate(board, board.current_player) ||
           insufficient_material(board) || is_fifty_move_rule(board) ||
           is_repetition(board);
}

bool DrawRules::is_stalemate(const Board &board, Color player) {
    if (CheckValidator::is_check(board, player)) {
        return false;
    }

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.grid_[y][x];
            if (piece.get_type() != PieceType::NONE &&
                piece.get_color() == player) {

                auto moves = MoveGenerator::get_legal_moves(board, {x, y});
                if (!moves.empty()) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool DrawRules::insufficient_material(const Board &board) {
    return has_insufficient_material(Color::WHITE, board) &&
           has_insufficient_material(Color::BLACK, board);
}

bool DrawRules::has_insufficient_material(Color color, const Board &board) {
    int pieces_count = 0;
    bool has_pawn = false;
    bool has_rook = false;
    bool has_queen = false;
    bool has_bishop = false;
    bool has_knight = false;

    for (const auto &row : board.grid_) {
        for (const auto &piece : row) {
            if (piece.get_type() != PieceType::NONE &&
                piece.get_color() == color) {
                pieces_count++;

                switch (piece.get_type()) {
                    case PieceType::PAWN:
                        has_pawn = true;
                        break;
                    case PieceType::ROOK:
                        has_rook = true;
                        break;
                    case PieceType::QUEEN:
                        has_queen = true;
                        break;
                    case PieceType::BISHOP:
                        has_bishop = true;
                        break;
                    case PieceType::KNIGHT:
                        has_knight = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    // King vs King
    if (pieces_count == 1)
        return true;

    // King + minor piece
    if (pieces_count == 2 && (has_bishop || has_knight))
        return true;

    // King + bishop vs King + bishop (same color bishops)
    if (pieces_count == 2 && has_bishop && is_bishop_vs_bishop(board)) {
        return true;
    }

    return false;
}

bool DrawRules::is_bishop_vs_bishop(const Board &board) {
    std::optional<std::pair<int, int>> white_bishop, black_bishop;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.grid_[y][x];
            if (piece.get_type() == PieceType::BISHOP) {
                if (piece.get_color() == Color::WHITE) {
                    white_bishop = {x, y};
                } else {
                    black_bishop = {x, y};
                }
            }
        }
    }

    if (white_bishop && black_bishop) {
        bool white_square =
            (white_bishop->first + white_bishop->second) % 2 == 0;
        bool black_square =
            (black_bishop->first + black_bishop->second) % 2 == 0;
        return white_square == black_square;
    }

    return false;
}

bool DrawRules::is_repetition(const Board &board) {
    if (board.position_history_.empty())
        return false;

    const std::string &current = board.position_history_.back();
    int count = 0;

    for (const auto &position : board.position_history_) {
        if (position == current) {
            count++;
            if (count >= 3) { // 3-fold repetition
                return true;
            }
        }
    }

    return false;
}

bool DrawRules::is_fifty_move_rule(const Board &board) {
    return board.halfmove_clock_ >= 50;
}

} // namespace chess
