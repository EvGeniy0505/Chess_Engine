#pragma once

#include "pieces/piece.hpp"
#include <array>
#include <utility>
#include <vector>

namespace chess {

class Board {
  public:
    Color current_player = Color::WHITE;
    Board(PieceSet set = PieceSet::UNICODE);

    // Game operations
    bool make_move(std::pair<int, int> from, std::pair<int, int> to,
                   PieceType promotion = PieceType::NONE);
    std::vector<std::pair<int, int>>
    get_legal_moves(std::pair<int, int> position) const;
    void print(bool show_highlights = false) const;

    // State queries
    bool is_check(Color player) const;
    bool is_checkmate(Color player);
    bool is_attacked(std::pair<int, int> square, Color by_color) const;
    bool is_empty(std::pair<int, int> square) const;
    bool is_enemy(std::pair<int, int> square, Color ally_color) const;

    // Accessors
    const Piece &get_piece(std::pair<int, int> square) const {
        return grid_[square.second][square.first];
    }
    PieceSet get_piece_set() const { return piece_set_; }
    void set_piece_set(PieceSet set) { piece_set_ = set; }

    void highlight_moves(const std::vector<std::pair<int, int>> &moves);
    void clear_highlights();

  private:
    std::array<std::array<Piece, 8>, 8> grid_;
    PieceSet piece_set_ = PieceSet::UNICODE;

    void reset_highlighted_squares();
    struct CastlingRights {
        bool white_kingside = true;
        bool white_queenside = true;
        bool black_kingside = true;
        bool black_queenside = true;
    } castling_rights_;

    // Internal helpers
    bool in_bounds(int x, int y) const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }

    // Friend classes for modular implementation
    friend class BoardInitializer;
    friend class MoveGenerator;
    friend class CastlingManager;
    friend class CheckValidator;
};
} // namespace chess
