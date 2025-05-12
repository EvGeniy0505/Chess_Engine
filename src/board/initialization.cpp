#include "board/initialization.hpp"
#include "../pieces/piece.hpp"

namespace chess {

void BoardInitializer::setup_initial_position(Board &board) {
    // Clear the board
    for (auto &row : board.grid_) {
        for (auto &square : row) {
            square = Piece();
        }
    }

    // Set up pawns
    for (int x = 0; x < 8; ++x) {
        board.grid_[1][x] = Piece(PieceType::PAWN, Color::BLACK);
        board.grid_[6][x] = Piece(PieceType::PAWN, Color::WHITE);
    }

    // Set up black pieces (back row)
    board.grid_[0][0] = Piece(PieceType::ROOK, Color::BLACK);
    board.grid_[0][1] = Piece(PieceType::KNIGHT, Color::BLACK);
    board.grid_[0][2] = Piece(PieceType::BISHOP, Color::BLACK);
    board.grid_[0][3] = Piece(PieceType::QUEEN, Color::BLACK);
    board.grid_[0][4] = Piece(PieceType::KING, Color::BLACK);
    board.grid_[0][5] = Piece(PieceType::BISHOP, Color::BLACK);
    board.grid_[0][6] = Piece(PieceType::KNIGHT, Color::BLACK);
    board.grid_[0][7] = Piece(PieceType::ROOK, Color::BLACK);

    // Set up white pieces (back row)
    board.grid_[7][0] = Piece(PieceType::ROOK, Color::WHITE);
    board.grid_[7][1] = Piece(PieceType::KNIGHT, Color::WHITE);
    board.grid_[7][2] = Piece(PieceType::BISHOP, Color::WHITE);
    board.grid_[7][3] = Piece(PieceType::QUEEN, Color::WHITE);
    board.grid_[7][4] = Piece(PieceType::KING, Color::WHITE);
    board.grid_[7][5] = Piece(PieceType::BISHOP, Color::WHITE);
    board.grid_[7][6] = Piece(PieceType::KNIGHT, Color::WHITE);
    board.grid_[7][7] = Piece(PieceType::ROOK, Color::WHITE);

    // Reset castling rights
    board.castling_rights_ = Board::CastlingRights{.white_kingside = true,
                                                   .white_queenside = true,
                                                   .black_kingside = true,
                                                   .black_queenside = true};

    // Set current player to White
    board.current_player = Color::WHITE;
}

} // namespace chess
