#pragma once

#include "pieces/piece.hpp"
#include <array>
#include <deque>
#include <optional>
#include <utility>
#include <vector>

namespace chess {

using Position = std::pair<int, int>;

namespace BoardInitializer {
extern const char *STANDARD_FEN;
}

class Board {
  public:
    Color current_player = Color::WHITE;
    explicit Board(const std::string &fen = BoardInitializer::STANDARD_FEN);

    // Game operations
    bool make_move(std::pair<int, int> from, std::pair<int, int> to,
                   PieceType promotion = PieceType::NONE);
    std::vector<std::pair<int, int>>
    get_legal_moves(std::pair<int, int> position) const;
    void print(bool show_highlights = false) const;

    // State queries
    bool is_check(Color player) const;
    bool is_checkmate(Color player);
    bool is_stalemate(Color player);
    bool is_draw() const;
    bool is_attacked(std::pair<int, int> square, Color by_color) const;
    bool is_empty(std::pair<int, int> square) const;
    bool is_enemy(std::pair<int, int> square, Color ally_color) const;

    std::optional<std::pair<int, int>> en_passant_target_;
    int halfmove_clock_ = 0;
    int fullmove_number_ = 1;

    // Accessors
    const Piece &get_piece(std::pair<int, int> square) const {
        return grid_[square.second][square.first];
    }
    PieceSet get_piece_set() const { return piece_set_; }
    void set_piece_set(PieceSet set) { piece_set_ = set; }

    Position find_king(Color color) const;

    void highlight_moves(const std::vector<std::pair<int, int>> &moves);
    void clear_highlights();

    struct CastlingRights {
        bool white_kingside = true;
        bool white_queenside = true;
        bool black_kingside = true;
        bool black_queenside = true;
    } castling_rights_;
    std::array<std::array<Piece, 8>, 8> grid_;

  private:
    PieceSet piece_set_ = PieceSet::UNICODE;
    std::deque<std::string> position_history_; // For repetition detection

    void reset_highlighted_squares();
    void add_position_to_history();

    bool in_bounds(int x, int y) const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }

    friend class MoveGenerator;
    friend class CastlingManager;
    friend class CheckValidator;
    friend class DrawRules;
};
} // namespace chess
