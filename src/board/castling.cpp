#include "board/castling.hpp"
#include "board/check.hpp"

namespace chess {

bool CastlingManager::try_perform_castle(Board &board,
                                         std::pair<int, int> king_from,
                                         std::pair<int, int> king_to) {
    const auto &piece = board.get_piece(king_from);
    if (piece.get_type() != PieceType::KING ||
        CheckValidator::is_check(board, piece.get_color()))
        return false;

    int direction = (king_to.first > king_from.first) ? 1 : -1;
    int rook_x = (direction > 0) ? 7 : 0;
    int rook_new_x = (direction > 0) ? king_to.first - 1 : king_to.first + 1;

    // Check rook exists and hasn't moved
    const auto &rook = board.grid_[king_from.second][rook_x];
    if (rook.get_type() != PieceType::ROOK ||
        rook.get_color() != piece.get_color())
        return false;

    // Check path is clear and not attacked
    for (int x = king_from.first; x != rook_x; x += direction) {
        if (x != king_from.first && !board.is_empty({x, king_from.second}))
            return false;

        if (x != rook_x &&
            CheckValidator::is_attacked(board, {x, king_from.second},
                                        piece.get_color() == Color::WHITE
                                            ? Color::BLACK
                                            : Color::WHITE))
            return false;
    }

    // Perform castling
    board.grid_[king_to.second][king_to.first] = piece;
    board.grid_[king_from.second][king_from.first] = Piece();
    board.grid_[king_from.second][rook_new_x] = rook;
    board.grid_[king_from.second][rook_x] = Piece();

    // Update castling rights
    if (piece.get_color() == Color::WHITE) {
        board.castling_rights_.white_kingside = false;
        board.castling_rights_.white_queenside = false;
    } else {
        board.castling_rights_.black_kingside = false;
        board.castling_rights_.black_queenside = false;
    }

    board.current_player =
        (board.current_player == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return true;
}

void CastlingManager::update_castling_rights(Board &board,
                                             std::pair<int, int> moved_from) {
    const auto &piece = board.get_piece(moved_from);

    if (piece.get_type() == PieceType::KING) {
        if (piece.get_color() == Color::WHITE) {
            board.castling_rights_.white_kingside = false;
            board.castling_rights_.white_queenside = false;
        } else {
            board.castling_rights_.black_kingside = false;
            board.castling_rights_.black_queenside = false;
        }
    } else if (piece.get_type() == PieceType::ROOK) {
        if (piece.get_color() == Color::WHITE) {
            if (moved_from.first == 0 && moved_from.second == 7)
                board.castling_rights_.white_queenside = false;
            else if (moved_from.first == 7 && moved_from.second == 7)
                board.castling_rights_.white_kingside = false;
        } else {
            if (moved_from.first == 0 && moved_from.second == 0)
                board.castling_rights_.black_queenside = false;
            else if (moved_from.first == 7 && moved_from.second == 0)
                board.castling_rights_.black_kingside = false;
        }
    }
}

bool CastlingManager::can_castle_kingside(const Board &board, Color color) {
    if (color == Color::WHITE) {
        return board.castling_rights_.white_kingside &&
               board.is_empty({5, 7}) && board.is_empty({6, 7}) &&
               !CheckValidator::is_attacked(board, {4, 7}, Color::BLACK) &&
               !CheckValidator::is_attacked(board, {5, 7}, Color::BLACK);
    } else {
        return board.castling_rights_.black_kingside &&
               board.is_empty({5, 0}) && board.is_empty({6, 0}) &&
               !CheckValidator::is_attacked(board, {4, 0}, Color::WHITE) &&
               !CheckValidator::is_attacked(board, {5, 0}, Color::WHITE);
    }
}

bool CastlingManager::can_castle_queenside(const Board &board, Color color) {
    if (color == Color::WHITE) {
        return board.castling_rights_.white_queenside &&
               board.is_empty({3, 7}) && board.is_empty({2, 7}) &&
               board.is_empty({1, 7}) &&
               !CheckValidator::is_attacked(board, {4, 7}, Color::BLACK) &&
               !CheckValidator::is_attacked(board, {3, 7}, Color::BLACK);
    } else {
        return board.castling_rights_.black_queenside &&
               board.is_empty({3, 0}) && board.is_empty({2, 0}) &&
               board.is_empty({1, 0}) &&
               !CheckValidator::is_attacked(board, {4, 0}, Color::WHITE) &&
               !CheckValidator::is_attacked(board, {3, 0}, Color::WHITE);
    }
}
} // namespace chess
