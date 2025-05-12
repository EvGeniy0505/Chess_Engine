#include "board.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

namespace chess {

Board::Board(PieceSet set) : piece_set_(set) { initializeBoard(); }

void Board::print(bool showHighlights) {
    std::cout << "\n   a  b  c  d  e  f  g  h\n";

    for (int y = 0; y < 8; ++y) {
        std::cout << 8 - y << " ";

        for (int x = 0; x < 8; ++x) {
            Piece &piece = grid_[y][x];
            CellColor cellColor = (x + y) % 2 ? CellColor::BLACK : CellColor::WHITE;

            if (showHighlights && piece.getType() == PieceType::HIGHLIGHT) {
                cellColor = (x + y) % 2 ? CellColor::HIGHLIGHT_BLACK : CellColor::HIGHLIGHT_WHITE;
            }

            piece.setCellColor(cellColor);
            std::cout << piece.getColoredSymbol(piece_set_);
        }

        std::cout << " " << 8 - y << "\n";
    }

    std::cout << "   a  b  c  d  e  f  g  h\n\n";
    std::cout << "Текущий игрок: " << (current_player_ == Color::WHITE ? "Белые" : "Чёрные") << "\n";
    if (isCheck(current_player_)) {
        std::cout << "ШАХ!\n";
    }
}

bool Board::makeMove(int fromX, int fromY, int toX, int toY, PieceType promotion) {
    if (!isInBounds(fromX, fromY) || !isInBounds(toX, toY)) return false;

    Piece &piece = grid_[fromY][fromX];
    if (piece.getType() == PieceType::NONE || piece.getColor() != current_player_) {
        return false;
    }

    if (piece.getType() == PieceType::KING && abs(fromX - toX) == 2) {
        return castle(fromX, fromY, toX, toY);
    }

    auto moves = generatePseudoLegalMoves(fromX, fromY);
    if (std::find(moves.begin(), moves.end(), std::make_pair(toX, toY)) == moves.end()) {
        return false;
    }

    if (piece.getType() == PieceType::PAWN && toX != fromX && isEmpty(toX, toY)) {
        grid_[fromY][toX] = Piece();
    }

    Piece movedPiece = piece;
    if (piece.getType() == PieceType::PAWN && (toY == 0 || toY == 7)) {
        movedPiece.setType(promotion == PieceType::NONE ? PieceType::QUEEN : promotion);
    }

    grid_[toY][toX] = movedPiece;
    grid_[fromY][fromX] = Piece();
    updateCastlingState(fromX, fromY);

    if (isCheck(current_player_)) {
        grid_[fromY][fromX] = piece;
        grid_[toY][toX] = Piece();
        return false;
    }

    current_player_ = (current_player_ == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return true;
}

bool Board::castle(int kingX, int kingY, int toX, int toY) {
    if (isCheck(current_player_))
        return false;

    int direction = (toX > kingX) ? 1 : -1;
    int rookX = (direction > 0) ? 7 : 0;

    if (grid_[kingY][rookX].getType() != PieceType::ROOK ||
        grid_[kingY][rookX].getColor() != current_player_) {
        return false;
    }

    for (int x = kingX + direction; x != rookX; x += direction) {
        if (!isEmpty(x, kingY))
            return false;
    }

    for (int x = kingX; x != toX + direction; x += direction) {
        if (isSquareAttacked(x, kingY,
                             current_player_ == Color::WHITE ? Color::BLACK
                                                             : Color::WHITE)) {
            return false;
        }
    }

    grid_[kingY][toX] = grid_[kingY][kingX];
    grid_[kingY][kingX] = Piece();

    int rookNewX = (direction > 0) ? toX - 1 : toX + 1;
    grid_[kingY][rookNewX] = grid_[kingY][rookX];
    grid_[kingY][rookX] = Piece();

    if (current_player_ == Color::WHITE) {
        white_king_moved_ = true;
        if (direction > 0)
            white_kingside_rook_moved_ = true;
        else
            white_queenside_rook_moved_ = true;
    } else {
        black_king_moved_ = true;
        if (direction > 0)
            black_kingside_rook_moved_ = true;
        else
            black_queenside_rook_moved_ = true;
    }

    current_player_ = (current_player_ == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return true;
}

void Board::updateCastlingState(int x, int y) {
    Piece &piece = grid_[y][x];
    if (piece.getType() == PieceType::KING) {
        if (piece.getColor() == Color::WHITE)
            white_king_moved_ = true;
        else
            black_king_moved_ = true;
    } else if (piece.getType() == PieceType::ROOK) {
        if (piece.getColor() == Color::WHITE) {
            if (x == 0 && y == 7)
                white_queenside_rook_moved_ = true;
            if (x == 7 && y == 7)
                white_kingside_rook_moved_ = true;
        } else {
            if (x == 0 && y == 0)
                black_queenside_rook_moved_ = true;
            if (x == 7 && y == 0)
                black_kingside_rook_moved_ = true;
        }
    }
}

bool Board::isSquareAttacked(int x, int y, Color byColor) const {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (grid_[i][j].getType() != PieceType::NONE &&
                grid_[i][j].getColor() == byColor) {
                auto moves = generatePseudoLegalMoves(j, i);
                if (std::find(moves.begin(), moves.end(),
                              std::make_pair(x, y)) != moves.end()) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::isCheck(Color player) const {
    int kingX = -1, kingY = -1;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (grid_[y][x].getType() == PieceType::KING &&
                grid_[y][x].getColor() == player) {
                kingX = x;
                kingY = y;
                break;
            }
        }
    }

    if (kingX == -1)
        return false;
    return isSquareAttacked(
        kingX, kingY, player == Color::WHITE ? Color::BLACK : Color::WHITE);
}

bool Board::isCheckmate(Color player) {
    if (!isCheck(player))
        return false;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (grid_[y][x].getType() != PieceType::NONE &&
                grid_[y][x].getColor() == player) {
                auto moves = generatePseudoLegalMoves(x, y);
                for (const auto &[mx, my] : moves) {
                    Board tempBoard = *this;
                    if (tempBoard.makeMove(x, y, mx, my)) {
                        if (!tempBoard.isCheck(player)) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

std::vector<std::pair<int, int>> Board::getPossibleMoves(int x, int y) const {
    std::vector<std::pair<int, int>> moves = generatePseudoLegalMoves(x, y);
    std::vector<std::pair<int, int>> validMoves;

    const Piece &piece = grid_[y][x];
    if (piece.getType() == PieceType::NONE || piece.getColor() != current_player_) {
        return validMoves;
    }

    for (const auto &[mx, my] : moves) {
        Board tempBoard = *this;
        if (tempBoard.makeMove(x, y, mx, my)) {
            validMoves.emplace_back(mx, my);
        }
    }

    if (piece.getType() == PieceType::KING && !isCheck(current_player_)) {
        if (canCastleKingside(current_player_)) {
            validMoves.emplace_back(x + 2, y);
        }
        if (canCastleQueenside(current_player_)) {
            validMoves.emplace_back(x - 2, y);
        }
    }

    return validMoves;
}

bool Board::canCastleKingside(Color player) const {
    if (player == Color::WHITE) {
        return !white_king_moved_ && !white_kingside_rook_moved_ &&
               isEmpty(5, 7) && isEmpty(6, 7) &&
               !isSquareAttacked(4, 7, Color::BLACK) &&
               !isSquareAttacked(5, 7, Color::BLACK);
    } else {
        return !black_king_moved_ && !black_kingside_rook_moved_ &&
               isEmpty(5, 0) && isEmpty(6, 0) &&
               !isSquareAttacked(4, 0, Color::WHITE) &&
               !isSquareAttacked(5, 0, Color::WHITE);
    }
}

bool Board::canCastleQueenside(Color player) const {
    if (player == Color::WHITE) {
        return !white_king_moved_ && !white_queenside_rook_moved_ &&
               isEmpty(3, 7) && isEmpty(2, 7) && isEmpty(1, 7) &&
               !isSquareAttacked(4, 7, Color::BLACK) &&
               !isSquareAttacked(3, 7, Color::BLACK);
    } else {
        return !black_king_moved_ && !black_queenside_rook_moved_ &&
               isEmpty(3, 0) && isEmpty(2, 0) && isEmpty(1, 0) &&
               !isSquareAttacked(4, 0, Color::WHITE) &&
               !isSquareAttacked(3, 0, Color::WHITE);
    }
}

void Board::highlightMoves(const std::vector<std::pair<int, int>> &moves) {
    clearHighlights();
    for (const auto &[x, y] : moves) {
        grid_[y][x] = Piece(PieceType::HIGHLIGHT, Color::WHITE);
    }
}

void Board::clearHighlights() {
    for (auto &row : grid_) {
        for (auto &square : row) {
            if (square.getType() == PieceType::HIGHLIGHT) {
                square = Piece();
            }
        }
    }
}

bool Board::isInBounds(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::isEmpty(int x, int y) const {
    return isInBounds(x, y) && grid_[y][x].getType() == PieceType::NONE;
}

bool Board::isEnemy(int x, int y, Color allyColor) const {
    return isInBounds(x, y) && grid_[y][x].getType() != PieceType::NONE &&
           grid_[y][x].getColor() != allyColor;
}

std::string Board::getPieceSymbol(PieceType type, Color color) const {
    static const std::string symbols[2][6] = {
        {"♙", "♘", "♗", "♖", "♕", "♔"}, // Белые
        {"♟", "♞", "♝", "♜", "♛", "♚"}  // Чёрные
    };

    if (type == PieceType::NONE)
        return ".";
    if (type == PieceType::HIGHLIGHT)
        return "*";

    int idx = static_cast<int>(type) - 1;
    return symbols[color == Color::WHITE ? 0 : 1][idx];
}

std::vector<std::pair<int, int>> Board::generatePseudoLegalMoves(int x, int y) const {
    std::vector<std::pair<int, int>> moves;
    if (!isInBounds(x, y))
        return moves;

    const Piece &piece = grid_[y][x];
    if (piece.getType() == PieceType::NONE)
        return moves;

    const Color enemyColor = piece.getColor() == Color::WHITE ? Color::BLACK : Color::WHITE;
    const int startRow = piece.getColor() == Color::WHITE ? 6 : 1;
    const int enPassantRow = piece.getColor() == Color::WHITE ? 3 : 4;

    switch (piece.getType()) {
        case PieceType::PAWN: {
            const int direction = piece.getColor() == Color::WHITE ? -1 : 1;

            if (isEmpty(x, y + direction)) {
                moves.emplace_back(x, y + direction);

                if (y == startRow && isEmpty(x, y + 2 * direction) &&
                    isEmpty(x, y + direction)) {
                    moves.emplace_back(x, y + 2 * direction);
                }
            }

            for (int dx : {-1, 1}) {
                if (x + dx < 0 || x + dx >= 8)
                    continue;

                if (isEnemy(x + dx, y + direction, piece.getColor())) {
                    moves.emplace_back(x + dx, y + direction);
                }

                if (y == enPassantRow) {
                    if (isEnemy(x + dx, y, piece.getColor()) &&
                        grid_[y][x + dx].getType() == PieceType::PAWN) {
                        moves.emplace_back(x + dx, y + direction);
                    }
                }
            }
            break;
        }

        case PieceType::KNIGHT: {
            constexpr std::array<std::pair<int, int>, 8> offsets = {
                {{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, 
                {1, -2}, {2, -1}, {-1, -2}, {-2, -1}}
            };

            for (const auto &[dx, dy] : offsets) {
                int nx = x + dx;
                int ny = y + dy;
                if (isInBounds(nx, ny) &&
                    (isEmpty(nx, ny) || isEnemy(nx, ny, piece.getColor()))) {
                    moves.emplace_back(nx, ny);
                }
            }
            break;
        }

        case PieceType::BISHOP: {
            constexpr std::array<std::pair<int, int>, 4> directions = {
                {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}
            };

            for (const auto &[dx, dy] : directions) {
                for (int step = 1; step < 8; ++step) {
                    int nx = x + dx * step;
                    int ny = y + dy * step;
                    if (!isInBounds(nx, ny))
                        break;

                    if (isEmpty(nx, ny)) {
                        moves.emplace_back(nx, ny);
                    } else if (isEnemy(nx, ny, piece.getColor())) {
                        moves.emplace_back(nx, ny);
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case PieceType::ROOK: {
            constexpr std::array<std::pair<int, int>, 4> directions = {
                {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
            };

            for (const auto &[dx, dy] : directions) {
                for (int step = 1; step < 8; ++step) {
                    int nx = x + dx * step;
                    int ny = y + dy * step;
                    if (!isInBounds(nx, ny))
                        break;

                    if (isEmpty(nx, ny)) {
                        moves.emplace_back(nx, ny);
                    } else if (isEnemy(nx, ny, piece.getColor())) {
                        moves.emplace_back(nx, ny);
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case PieceType::QUEEN: {
            constexpr std::array<std::pair<int, int>, 8> directions = {
                {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                 {1, 0}, {-1, 0}, {0, 1}, {0, -1}}
            };

            for (const auto &[dx, dy] : directions) {
                for (int step = 1; step < 8; ++step) {
                    int nx = x + dx * step;
                    int ny = y + dy * step;
                    if (!isInBounds(nx, ny))
                        break;

                    if (isEmpty(nx, ny)) {
                        moves.emplace_back(nx, ny);
                    } else if (isEnemy(nx, ny, piece.getColor())) {
                        moves.emplace_back(nx, ny);
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case PieceType::KING: {
            constexpr std::array<std::pair<int, int>, 8> offsets = {
                {{1, 1}, {1, 0}, {1, -1}, {0, 1}, 
                {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}}
            };

            for (const auto &[dx, dy] : offsets) {
                int nx = x + dx;
                int ny = y + dy;
                if (isInBounds(nx, ny) &&
                    (isEmpty(nx, ny) || isEnemy(nx, ny, piece.getColor()))) {
                    moves.emplace_back(nx, ny);
                }
            }
            break;
        }

        default:
            break;
    }

    return moves;
}

void Board::initializeBoard() {
    // Initialize empty squares
    for (auto &row : grid_) {
        for (auto &square : row) {
            square = Piece(PieceType::NONE, Color::WHITE);
        }
    }

    // Initialize pawns
    for (int x = 0; x < 8; ++x) {
        grid_[1][x] = Piece(PieceType::PAWN, Color::BLACK);
        grid_[6][x] = Piece(PieceType::PAWN, Color::WHITE);
    }

    // Initialize black pieces
    grid_[0][0] = Piece(PieceType::ROOK, Color::BLACK);
    grid_[0][1] = Piece(PieceType::KNIGHT, Color::BLACK);
    grid_[0][2] = Piece(PieceType::BISHOP, Color::BLACK);
    grid_[0][3] = Piece(PieceType::QUEEN, Color::BLACK);
    grid_[0][4] = Piece(PieceType::KING, Color::BLACK);
    grid_[0][5] = Piece(PieceType::BISHOP, Color::BLACK);
    grid_[0][6] = Piece(PieceType::KNIGHT, Color::BLACK);
    grid_[0][7] = Piece(PieceType::ROOK, Color::BLACK);

    // Initialize white pieces
    grid_[7][0] = Piece(PieceType::ROOK, Color::WHITE);
    grid_[7][1] = Piece(PieceType::KNIGHT, Color::WHITE);
    grid_[7][2] = Piece(PieceType::BISHOP, Color::WHITE);
    grid_[7][3] = Piece(PieceType::QUEEN, Color::WHITE);
    grid_[7][4] = Piece(PieceType::KING, Color::WHITE);
    grid_[7][5] = Piece(PieceType::BISHOP, Color::WHITE);
    grid_[7][6] = Piece(PieceType::KNIGHT, Color::WHITE);
    grid_[7][7] = Piece(PieceType::ROOK, Color::WHITE);

    // Reset castling flags
    white_king_moved_ = false;
    white_kingside_rook_moved_ = false;
    white_queenside_rook_moved_ = false;
    black_king_moved_ = false;
    black_kingside_rook_moved_ = false;
    black_queenside_rook_moved_ = false;
}

} // namespace chess
