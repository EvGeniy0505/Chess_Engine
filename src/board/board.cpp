#include "board/board.hpp"
#include "board/castling.hpp"
#include "board/check.hpp"
#include "board/initialization.hpp"
#include "board/move_generation.hpp"
#include <algorithm>
#include <iostream>

namespace chess {

Board::Board(PieceSet set) : piece_set_(set) {
    BoardInitializer::setup_initial_position(*this);
}

bool Board::make_move(std::pair<int, int> from, std::pair<int, int> to,
                      PieceType promotion) {
    // Добавьте проверку на выход за границы
    if (!in_bounds(from.first, from.second) ||
        !in_bounds(to.first, to.second)) {
        return false;
    }

    // Используйте get_piece вместо прямого доступа к grid_
    const Piece &piece = get_piece(from);
    if (piece.get_type() == PieceType::NONE ||
        piece.get_color() != current_player) {
        return false;
    }

    // Handle castling
    if (piece.get_type() == PieceType::KING &&
        abs(from.first - to.first) == 2) {
        return CastlingManager::try_perform_castle(*this, from, to);
    }

    auto legal_moves = get_legal_moves(from);
    auto it = std::find_if(legal_moves.begin(), legal_moves.end(),
                           [&to](const std::pair<int, int> &move) {
                               return move.first == to.first &&
                                      move.second == to.second;
                           });

    if (it == legal_moves.end()) {
        return false;
    }

    // Handle en passant
    if (piece.get_type() == PieceType::PAWN && from.first != to.first &&
        is_empty(to)) {
        grid_[from.second][to.first] = Piece();
    }

    // Handle promotion
    Piece moved_piece = piece;
    if (piece.get_type() == PieceType::PAWN &&
        (to.second == 0 || to.second == 7)) {
        moved_piece.set_type(promotion == PieceType::NONE ? PieceType::QUEEN
                                                          : promotion);
    }

    // Execute move
    grid_[to.second][to.first] = moved_piece;
    grid_[from.second][from.first] = Piece();
    CastlingManager::update_castling_rights(*this, from);

    // Check for self-check
    if (CheckValidator::is_check(*this, current_player)) {
        // Rollback move
        grid_[from.second][from.first] = piece;
        grid_[to.second][to.first] = Piece();
        return false;
    }

    current_player =
        (current_player == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return true;
}

std::vector<std::pair<int, int>>
Board::get_legal_moves(std::pair<int, int> position) const {
    return MoveGenerator::get_legal_moves(*this, position);
}

bool Board::is_check(Color player) const {
    return CheckValidator::is_check(*this, player);
}

bool Board::is_checkmate(Color player) {
    return CheckValidator::is_checkmate(*this, player);
}

bool Board::is_attacked(std::pair<int, int> square, Color by_color) const {
    return CheckValidator::is_attacked(*this, square, by_color);
}

bool Board::is_empty(std::pair<int, int> square) const {
    return in_bounds(square.first, square.second) &&
           grid_[square.second][square.first].get_type() == PieceType::NONE;
}

bool Board::is_enemy(std::pair<int, int> square, Color ally_color) const {
    return in_bounds(square.first, square.second) &&
           grid_[square.second][square.first].get_type() != PieceType::NONE &&
           grid_[square.second][square.first].get_color() != ally_color;
}

void Board::print(bool show_highlights) const {
    std::cout << "\n   a  b  c  d  e  f  g  h\n";
    for (int y = 0; y < 8; ++y) {
        std::cout << 8 - y << " ";
        for (int x = 0; x < 8; ++x) {
            const Piece &piece = grid_[y][x];
            CellColor cell_color =
                (x + y) % 2 ? CellColor::BLACK : CellColor::WHITE;

            // Если включена подсветка и это клетка подсветки
            if (show_highlights && piece.get_type() == PieceType::HIGHLIGHT) {
                cell_color = (x + y) % 2 ? CellColor::HIGHLIGHT_BLACK
                                         : CellColor::HIGHLIGHT_WHITE;
            }

            // Создаем временную фигуру с правильным цветом клетки
            Piece temp_piece = piece;
            temp_piece.set_cell_color(cell_color);
            std::cout << temp_piece.getColoredSymbol(piece_set_);
        }
        std::cout << " " << 8 - y << "\n";
    }
    std::cout << "   a  b  c  d  e  f  g  h\n\n";
    std::cout << "Current player: "
              << (current_player == Color::WHITE ? "White" : "Black") << "\n";
    if (is_check(current_player)) {
        std::cout << "CHECK!\n";
    }
}

void Board::highlight_moves(const std::vector<std::pair<int, int>> &moves) {
    clear_highlights();
    for (const auto &[x, y] : moves) {
        if (in_bounds(x, y)) {
            // Подсвечиваем только пустые клетки или клетки с вражескими фигурами
            // 
            if (is_empty({x, y}) || is_enemy({x, y}, current_player)) {
                grid_[y][x] = Piece(PieceType::HIGHLIGHT, Color::WHITE);
            }
        }
    }
}

void Board::clear_highlights() {
    for (auto &row : grid_) {
        for (auto &square : row) {
            if (square.get_type() == PieceType::HIGHLIGHT) {
                square = Piece();
            }
        }
    }
}

void Board::reset_highlighted_squares() {
    for (auto &row : grid_) {
        for (auto &square : row) {
            if (square.get_type() == PieceType::HIGHLIGHT) {
                square = Piece();
            }
        }
    }
}
} // namespace chess
