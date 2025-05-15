#pragma once
#include "board/board.hpp"
#include "pieces/piece.hpp"
#include <string>

namespace chess {

namespace BoardInitializer {
extern const char *STANDARD_FEN;
extern const char *EMPTY_FEN;
extern const char *TEST_POSITION_FEN;

void setup_initial_position(Board &board,
                            const std::string &fen = STANDARD_FEN);
std::string export_to_fen(const Board &board);

namespace detail {
void parse_piece_placement(Board &board, const std::string &fen_part);
void parse_active_color(Board &board, const std::string &active_color);
void parse_castling_rights(Board &board, const std::string &castling);
void parse_en_passant(Board &board, const std::string &en_passant);
void parse_halfmove_clock(Board &board, const std::string &clock);
void parse_fullmove_number(Board &board, const std::string &number);
char piece_to_char(const Piece &piece);
Piece char_to_piece(char c);
} // namespace detail
} // namespace BoardInitializer

} // namespace chess
