#include "board/board.hpp"
#include "board/castling.hpp"
#include "board/check.hpp"
#include "board/draw_rules.hpp"
#include "board/initialization.hpp"
#include "board/move_generation.hpp"
#include <algorithm>
#include <iostream>

namespace chess {

Board::Board(const std::string &fen) {
    BoardInitializer::setup_initial_position(*this, fen);
    add_position_to_history();
}

void Board::add_position_to_history() {
    position_history_.push_back(BoardInitializer::export_to_fen(*this));
    if (position_history_.size() > 8) { // Keep last 8 positions (half-moves)
        position_history_.pop_front();
    }
}

bool Board::make_move(std::pair<int, int> from, std::pair<int, int> to,
                      PieceType promotion) {
    if (!in_bounds(from.first, from.second) ||
        !in_bounds(to.first, to.second)) {
        return false;
    }

    const Piece &piece = get_piece(from);
    if (piece.get_type() == PieceType::NONE ||
        piece.get_color() != current_player) {
        return false;
    }

    // Handle castling
    if (piece.get_type() == PieceType::KING &&
        abs(from.first - to.first) == 2) {
        bool success = CastlingManager::try_perform_castle(*this, from, to);
        if (success) {
            add_position_to_history();
        }
        return success;
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
        is_empty(to) && en_passant_target_ && to == *en_passant_target_) {
        // Remove the captured pawn
        grid_[from.second][to.first] = Piece();
    }

    // Update en passant target
    if (piece.get_type() == PieceType::PAWN &&
        abs(from.second - to.second) == 2) {
        // Для белых пешек: поле за пешкой (ниже по доске)
        // Для чёрных пешек: поле за пешкой (выше по доске)
        int direction = piece.get_color() == Color::WHITE ? -1 : 1;
        en_passant_target_ = {from.first, from.second + direction};
    } else {
        en_passant_target_ = std::nullopt;
    }

    // Handle promotion
    Piece moved_piece = piece;
    if (piece.get_type() == PieceType::PAWN &&
        (to.second == 0 || to.second == 7)) {
        moved_piece.set_type(promotion == PieceType::NONE ? PieceType::QUEEN
                                                          : promotion);
    }

    // Save previous state for halfmove clock
    bool reset_halfmove =
        (piece.get_type() == PieceType::PAWN) ||
        (!is_empty(to) || (en_passant_target_ && to == *en_passant_target_));

    // Execute move
    grid_[to.second][to.first] = moved_piece;
    grid_[from.second][from.first] = Piece();
    CastlingManager::update_castling_rights(*this, from);

    // Update halfmove clock and fullmove number
    if (reset_halfmove) {
        halfmove_clock_ = 0;
    } else {
        halfmove_clock_++;
    }

    if (current_player == Color::BLACK) {
        fullmove_number_++;
    }

    // Check for self-check
    if (CheckValidator::is_check(*this, current_player)) {
        // Rollback move
        grid_[from.second][from.first] = piece;
        grid_[to.second][to.first] = Piece();
        return false;
    }

    current_player =
        (current_player == Color::WHITE) ? Color::BLACK : Color::WHITE;
    add_position_to_history();
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

bool Board::is_draw() const { return DrawRules::is_draw(*this); }

bool Board::is_stalemate(Color player) {
    return DrawRules::is_stalemate(*this, player);
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

            if (show_highlights && piece.get_type() == PieceType::HIGHLIGHT) {
                cell_color = (x + y) % 2 ? CellColor::HIGHLIGHT_BLACK
                                         : CellColor::HIGHLIGHT_WHITE;
            }

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
    if (is_draw()) {
        std::cout << "DRAW!\n";
    }
}

void Board::highlight_moves(const std::vector<std::pair<int, int>> &moves) {
    clear_highlights();
    for (const auto &[x, y] : moves) {
        if (in_bounds(x, y)) {
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

Position Board::find_king(Color color) const {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position pos{x, y};
            const auto &piece = get_piece(pos);
            if (piece.get_type() == PieceType::KING &&
                piece.get_color() == color) {
                return pos;
            }
        }
    }
    return {-1, -1}; // В корректной позиции этого не должно происходить
}

void Board::reset_highlighted_squares() { clear_highlights(); }
} // namespace chess
