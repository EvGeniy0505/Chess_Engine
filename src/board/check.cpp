#include "board/check.hpp"
#include "board/move_generation.hpp"
#include <algorithm>

namespace chess {

bool CheckValidator::is_check(const Board &board, Color player) {
    // Find king position
    std::pair<int, int> king_pos = {-1, -1};
    for (int y = 0; y < 8 && king_pos.first == -1; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.grid_[y][x];
            if (piece.get_type() == PieceType::KING &&
                piece.get_color() == player) {
                king_pos = {x, y};
                break;
            }
        }
    }

    if (king_pos.first == -1)
        return false;
    return is_attacked(board, king_pos,
                       player == Color::WHITE ? Color::BLACK : Color::WHITE);
}

bool CheckValidator::is_checkmate(Board &board, Color player) {
    if (!is_check(board, player))
        return false;

    // Check if any move can get out of check
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.grid_[y][x];
            if (piece.get_type() != PieceType::NONE &&
                piece.get_color() == player) {
                auto moves =
                    MoveGenerator::generate_pseudo_legal_moves(board, {x, y});
                for (const auto &move : moves) {
                    Board temp = board;
                    if (temp.make_move({x, y}, move)) {
                        if (!CheckValidator::is_check(temp, player)) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool CheckValidator::is_stalemate(Board &board, Color player) {
    // Условия пата:
    // 1. Нет шаха
    // 2. Нет легальных ходов
    if (is_check(board, player)) {
        return false;
    }

    // Проверяем все возможные ходы
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const auto &piece = board.grid_[y][x];
            if (piece.get_type() != PieceType::NONE && 
                piece.get_color() == player) {
                
                auto moves = MoveGenerator::get_legal_moves(board, {x, y});
                if (!moves.empty()) {
                    return false;  // Нашли хотя бы один легальный ход
                }
            }
        }
    }

    return true;  // Нет легальных ходов - пат
}

bool CheckValidator::is_attacked(const Board &board, std::pair<int, int> square,
                                 Color by_color) {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            // Используйте get_piece вместо прямого доступа к grid_
            const auto &piece = board.get_piece({x, y});
            if (piece.get_type() != PieceType::NONE &&
                piece.get_color() == by_color) {
                auto moves =
                    MoveGenerator::generate_pseudo_legal_moves(board, {x, y});
                auto it = std::find_if(
                    moves.begin(), moves.end(),
                    [&square](const auto &move) { return move == square; });
                if (it != moves.end()) {
                    return true;
                }
            }
        }
    }
    return false;
}
} // namespace chess
