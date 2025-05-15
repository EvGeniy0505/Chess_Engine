#include "board/move_generation.hpp"
#include "board/castling.hpp"
#include "board/check.hpp"
#include <algorithm>
#include <iostream>

namespace chess {
namespace {

void add_pawn_moves(const Board &board, std::vector<std::pair<int, int>> &moves,
                    std::pair<int, int> pos) {
    const auto &piece = board.get_piece(pos);
    int direction = piece.get_color() == Color::WHITE ? -1 : 1;
    int start_row = piece.get_color() == Color::WHITE ? 6 : 1;
    int en_passant_row = piece.get_color() == Color::WHITE ? 3 : 4;

    // Forward moves
    if (board.is_empty({pos.first, pos.second + direction})) {
        moves.emplace_back(pos.first, pos.second + direction);

        if (pos.second == start_row &&
            board.is_empty({pos.first, pos.second + 2 * direction})) {
            moves.emplace_back(pos.first, pos.second + 2 * direction);
        }
    }

    // Captures
    for (int dx : {-1, 1}) {
        int x = pos.first + dx;
        int y = pos.second + direction;

        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            // Regular capture
            if (board.is_enemy({x, y}, piece.get_color())) {
                moves.emplace_back(x, y);
            }

            //
            // En passant capture
            if (pos.second == en_passant_row && board.en_passant_target_ &&
                x == board.en_passant_target_->first &&
                y == board.en_passant_target_->second) {
                moves.emplace_back(x, y);
            }
        }
    }
}

void add_knight_moves(const Board &board,
                      std::vector<std::pair<int, int>> &moves,
                      std::pair<int, int> pos) {
    static const std::array<std::pair<int, int>, 8> offsets = {{{1, 2},
                                                                {2, 1},
                                                                {-1, 2},
                                                                {-2, 1},
                                                                {1, -2},
                                                                {2, -1},
                                                                {-1, -2},
                                                                {-2, -1}}};

    const auto &piece = board.get_piece(pos);
    for (const auto &[dx, dy] : offsets) {
        int x = pos.first + dx;
        int y = pos.second + dy;

        if (x >= 0 && x < 8 && y >= 0 && y < 8 &&
            (board.is_empty({x, y}) ||
             board.is_enemy({x, y}, piece.get_color()))) {
            moves.emplace_back(x, y);
        }
    }
}

void add_king_moves(const Board &board, std::vector<std::pair<int, int>> &moves,
                    std::pair<int, int> pos) {
    static const std::array<std::pair<int, int>, 8> directions = {
        {{1, 1},     // вправо-вверх
         {1, 0},     // вправо
         {1, -1},    // вправо-вниз
         {0, 1},     // вверх
         {0, -1},    // вниз
         {-1, 1},    // влево-вверх  ← ЭТОТ ХОД РАНЬШЕ НЕ РАБОТАЛ
         {-1, 0},    // влево
         {-1, -1}}}; // влево-вниз

    const auto &piece = board.get_piece(pos);
    for (const auto &[dx, dy] : directions) {
        int x = pos.first + dx;
        int y = pos.second + dy;

        // Проверяем границы доски
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
            continue;
        }

        // Клетка должна быть пустой или содержать вражескую фигуру
        if (board.is_empty({x, y}) ||
            board.is_enemy({x, y}, piece.get_color())) {
            moves.emplace_back(x, y);
        }
    }
}

template <typename DirIter>
void add_sliding_moves(const Board &board,
                       std::vector<std::pair<int, int>> &moves,
                       std::pair<int, int> pos, DirIter begin, DirIter end) {
    const auto &piece = board.get_piece(pos);
    for (auto it = begin; it != end; ++it) {
        for (int step = 1; step < 8; ++step) {
            int x = pos.first + it->first * step;
            int y = pos.second + it->second * step;

            if (x < 0 || x >= 8 || y < 0 || y >= 8)
                break;

            if (board.is_empty({x, y})) {
                moves.emplace_back(x, y);
            } else {
                if (board.is_enemy({x, y}, piece.get_color())) {
                    moves.emplace_back(x, y);
                }
                break;
            }
        }
    }
}
} // namespace

std::vector<std::pair<int, int>>
MoveGenerator::generate_pseudo_legal_moves(const Board &board,
                                           std::pair<int, int> pos) {
    std::vector<std::pair<int, int>> moves;
    const auto &piece = board.get_piece(pos);
    if (piece.get_type() == PieceType::NONE)
        return moves;

    switch (piece.get_type()) {
        case PieceType::PAWN:
            add_pawn_moves(board, moves, pos);
            break;

        case PieceType::KNIGHT:
            add_knight_moves(board, moves, pos);
            break;

        case PieceType::BISHOP: {
            static const std::array<std::pair<int, int>, 4> bishop_dirs = {
                {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
            add_sliding_moves(board, moves, pos, bishop_dirs.begin(),
                              bishop_dirs.end());
            break;
        }

        case PieceType::ROOK: {
            static const std::array<std::pair<int, int>, 4> rook_dirs = {
                {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
            add_sliding_moves(board, moves, pos, rook_dirs.begin(),
                              rook_dirs.end());
            break;
        }

        case PieceType::QUEEN: {
            static const std::array<std::pair<int, int>, 8> queen_dirs = {
                {{1, 1},
                 {1, -1},
                 {-1, 1},
                 {-1, -1},
                 {1, 0},
                 {-1, 0},
                 {0, 1},
                 {0, -1}}};
            add_sliding_moves(board, moves, pos, queen_dirs.begin(),
                              queen_dirs.end());
            break;
        }

        case PieceType::KING: {
            add_king_moves(board, moves, pos);
            break;
        }

        default:
            break;
    }

    return moves;
}

std::vector<std::pair<int, int>>
MoveGenerator::get_legal_moves(const Board &board, std::pair<int, int> pos) {
    auto pseudo_legal = generate_pseudo_legal_moves(board, pos);
    std::vector<std::pair<int, int>> legal_moves;
    const auto &piece = board.get_piece(pos);

    Board temp_board = board;
    temp_board.current_player = piece.get_color();

    for (const auto &move : pseudo_legal) {
        // Save original state
        Piece original_from = temp_board.get_piece(pos);
        Piece original_to = temp_board.get_piece(move);
        auto original_en_passant = temp_board.en_passant_target_;
        auto original_castling = temp_board.castling_rights_;

        // Handle en passant capture
        if (piece.get_type() == PieceType::PAWN && pos.first != move.first &&
            board.is_empty(move) && board.en_passant_target_) {
            int direction = piece.get_color() == Color::WHITE ? -1 : 1;
            // Проверяем, что идём на поле за целью en passant
            if (move.first == board.en_passant_target_->first &&
                move.second == board.en_passant_target_->second - direction) {
                // Удаляем пешку с поля en_passant_target_
                temp_board.grid_[board.en_passant_target_->second]
                                [board.en_passant_target_->first] = Piece();
            }
        }

        // Execute move
        temp_board.grid_[move.second][move.first] = original_from;
        temp_board.grid_[pos.second][pos.first] = Piece();

        // Update castling rights
        if (piece.get_type() == PieceType::KING ||
            piece.get_type() == PieceType::ROOK) {
            CastlingManager::update_castling_rights(temp_board, pos);
        }

        if (!CheckValidator::is_check(temp_board, piece.get_color())) {
            legal_moves.push_back(move);
        }

        // Restore original state
        temp_board.grid_[pos.second][pos.first] = original_from;
        temp_board.grid_[move.second][move.first] = original_to;
        temp_board.en_passant_target_ = original_en_passant;
        temp_board.castling_rights_ = original_castling;
    }

    // Add castling moves
    if (piece.get_type() == PieceType::KING &&
        !CheckValidator::is_check(board, piece.get_color())) {
        if (CastlingManager::can_castle_kingside(board, piece.get_color())) {
            legal_moves.emplace_back(pos.first + 2, pos.second);
        }
        if (CastlingManager::can_castle_queenside(board, piece.get_color())) {
            legal_moves.emplace_back(pos.first - 2, pos.second);
        }
    }

    return legal_moves;
}
} // namespace chess
