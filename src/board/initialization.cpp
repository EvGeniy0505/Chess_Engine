#include "board/initialization.hpp"
#include <algorithm>
#include <cctype>
#include <optional>
#include <sstream>
#include <stdexcept>

namespace chess {
namespace BoardInitializer {

const char *STANDARD_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const char *EMPTY_FEN = "8/8/8/8/8/8/8/8 w - - 0 1";
const char *TEST_POSITION_FEN =
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

namespace detail {

Piece char_to_piece(char c) {
    Color color = isupper(c) ? Color::WHITE : Color::BLACK;
    c = tolower(c);

    switch (c) {
        case 'p':
            return Piece(PieceType::PAWN, color);
        case 'n':
            return Piece(PieceType::KNIGHT, color);
        case 'b':
            return Piece(PieceType::BISHOP, color);
        case 'r':
            return Piece(PieceType::ROOK, color);
        case 'q':
            return Piece(PieceType::QUEEN, color);
        case 'k':
            return Piece(PieceType::KING, color);
        default:
            throw std::invalid_argument("Invalid FEN piece character: " +
                                        std::string(1, c));
    }
}

char piece_to_char(const Piece &piece) {
    if (piece.get_type() == PieceType::NONE)
        return '1';

    char c;
    switch (piece.get_type()) {
        case PieceType::PAWN:
            c = 'p';
            break;
        case PieceType::KNIGHT:
            c = 'n';
            break;
        case PieceType::BISHOP:
            c = 'b';
            break;
        case PieceType::ROOK:
            c = 'r';
            break;
        case PieceType::QUEEN:
            c = 'q';
            break;
        case PieceType::KING:
            c = 'k';
            break;
        default:
            return '1';
    }

    return piece.get_color() == Color::WHITE ? toupper(c) : c;
}

} // namespace detail

void setup_initial_position(Board &board, const std::string &fen) {
    std::istringstream fen_stream(fen);
    std::string part;

    // 1. Piece placement
    if (!std::getline(fen_stream, part, ' ')) {
        throw std::invalid_argument("Invalid FEN: missing piece placement");
    }
    detail::parse_piece_placement(board, part);

    // 2. Active color
    if (!std::getline(fen_stream, part, ' ')) {
        throw std::invalid_argument("Invalid FEN: missing active color");
    }
    detail::parse_active_color(board, part);

    // 3. Castling availability
    if (!std::getline(fen_stream, part, ' ')) {
        throw std::invalid_argument("Invalid FEN: missing castling rights");
    }
    detail::parse_castling_rights(board, part);

    // 4. En passant
    if (!std::getline(fen_stream, part, ' ')) {
        throw std::invalid_argument("Invalid FEN: missing en passant");
    }
    detail::parse_en_passant(board, part);

    // 5. Halfmove clock
    if (!std::getline(fen_stream, part, ' ')) {
        throw std::invalid_argument("Invalid FEN: missing halfmove clock");
    }
    detail::parse_halfmove_clock(board, part);

    // 6. Fullmove number
    if (std::getline(fen_stream, part, ' ')) {
        detail::parse_fullmove_number(board, part);
    }
}

void detail::parse_piece_placement(Board &board, const std::string &fen_part) {
    int rank = 0;
    int file = 0;

    for (char c : fen_part) {
        if (c == '/') {
            if (file != 8) {
                throw std::invalid_argument(
                    "Invalid FEN: incomplete rank in piece placement");
            }
            rank++;
            file = 0;
            if (rank >= 8)
                break;
        } else if (isdigit(c)) {
            int spaces = c - '0';
            if (spaces < 1 || spaces > 8) {
                throw std::invalid_argument(
                    "Invalid FEN: invalid number in piece placement");
            }
            file += spaces;
            if (file > 8) {
                throw std::invalid_argument(
                    "Invalid FEN: too many squares in rank");
            }
        } else {
            if (file >= 8) {
                throw std::invalid_argument(
                    "Invalid FEN: too many pieces in rank");
            }
            board.grid_[rank][file] = detail::char_to_piece(c);
            file++;
        }
    }

    if (rank != 7 || file != 8) {
        throw std::invalid_argument("Invalid FEN: incomplete board");
    }
}

void detail::parse_active_color(Board &board, const std::string &active_color) {
    if (active_color.size() != 1) {
        throw std::invalid_argument(
            "Invalid FEN: active color must be 'w' or 'b'");
    }

    switch (active_color[0]) {
        case 'w':
            board.current_player = Color::WHITE;
            break;
        case 'b':
            board.current_player = Color::BLACK;
            break;
        default:
            throw std::invalid_argument(
                "Invalid FEN: active color must be 'w' or 'b'");
    }
}

void detail::parse_castling_rights(Board &board, const std::string &castling) {
    board.castling_rights_ = Board::CastlingRights{};

    if (castling == "-") {
        return;
    }

    for (char c : castling) {
        switch (c) {
            case 'K':
                board.castling_rights_.white_kingside = true;
                break;
            case 'Q':
                board.castling_rights_.white_queenside = true;
                break;
            case 'k':
                board.castling_rights_.black_kingside = true;
                break;
            case 'q':
                board.castling_rights_.black_queenside = true;
                break;
            default:
                throw std::invalid_argument(
                    "Invalid FEN: invalid castling right");
        }
    }
}

void detail::parse_en_passant(Board &board, const std::string &en_passant) {
    if (en_passant == "-") {
        board.en_passant_target_ = std::nullopt;
        return;
    }

    if (en_passant.size() != 2) {
        throw std::invalid_argument(
            "Invalid FEN: en passant must be '-' or square coordinate");
    }

    int file = tolower(en_passant[0]) - 'a';
    int rank = 8 - (en_passant[1] - '0');

    if (file < 0 || file >= 8 || rank < 0 || rank >= 8) {
        throw std::invalid_argument("Invalid FEN: invalid en passant square");
    }

    board.en_passant_target_ = {file, rank};
}

void detail::parse_halfmove_clock(Board &board, const std::string &clock) {
    try {
        board.halfmove_clock_ = std::stoi(clock);
        if (board.halfmove_clock_ < 0) {
            throw std::invalid_argument("Halfmove clock cannot be negative");
        }
    } catch (...) {
        throw std::invalid_argument(
            "Invalid FEN: halfmove clock must be a number");
    }
}

void detail::parse_fullmove_number(Board &board, const std::string &number) {
    try {
        board.fullmove_number_ = std::stoi(number);
        if (board.fullmove_number_ < 1) {
            throw std::invalid_argument("Fullmove number must be at least 1");
        }
    } catch (...) {
        throw std::invalid_argument(
            "Invalid FEN: fullmove number must be a positive integer");
    }
}

std::string export_to_fen(const Board &board) {
    std::ostringstream fen;

    // 1. Piece placement
    for (int rank = 0; rank < 8; ++rank) {
        int empty_count = 0;

        for (int file = 0; file < 8; ++file) {
            const Piece &piece = board.grid_[rank][file];

            if (piece.get_type() == PieceType::NONE) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen << empty_count;
                    empty_count = 0;
                }
                fen << detail::piece_to_char(piece);
            }
        }

        if (empty_count > 0) {
            fen << empty_count;
        }

        if (rank < 7) {
            fen << '/';
        }
    }

    // 2. Active color
    fen << ' ' << (board.current_player == Color::WHITE ? 'w' : 'b');

    // 3. Castling rights
    fen << ' ';
    bool any_castling = false;

    if (board.castling_rights_.white_kingside) {
        fen << 'K';
        any_castling = true;
    }
    if (board.castling_rights_.white_queenside) {
        fen << 'Q';
        any_castling = true;
    }
    if (board.castling_rights_.black_kingside) {
        fen << 'k';
        any_castling = true;
    }
    if (board.castling_rights_.black_queenside) {
        fen << 'q';
        any_castling = true;
    }

    if (!any_castling)
        fen << '-';

    // 4. En passant
    fen << ' ';
    if (board.en_passant_target_) {
        char file = 'a' + board.en_passant_target_->first;
        char rank = '8' - board.en_passant_target_->second;
        fen << file << rank;
    } else {
        fen << '-';
    }

    // 5. Halfmove clock
    fen << ' ' << board.halfmove_clock_;

    // 6. Fullmove number
    fen << ' ' << board.fullmove_number_;

    return fen.str();
}

} // namespace BoardInitializer
} // namespace chess
