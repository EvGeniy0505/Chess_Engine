#include "board.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

namespace chess {

Board::Board() {
    initializeBoard();
}

void Board::print(bool showHighlights) const {
    std::cout << "  a b c d e f g h\n";
    for (int y = 0; y < 8; ++y) {
        std::cout << 8 - y << " ";
        for (int x = 0; x < 8; ++x) {
            std::cout << grid_[y][x].symbol() << " ";
        }
        std::cout << 8 - y << "\n";
    }
    std::cout << "  a b c d e f g h\n";
    std::cout << "Current player: " << (current_player_ == Color::White ? "White" : "Black") << "\n";
    if (isCheck(current_player_)) {
        std::cout << "CHECK!\n";
    }
}

bool Board::makeMove(int fromX, int fromY, int toX, int toY, PieceType promotion) {
    if (!isInBounds(fromX, fromY) || !isInBounds(toX, toY)) {
        return false;
    }

    Piece& piece = grid_[fromY][fromX];
    if (piece.type == PieceType::None || piece.color != current_player_) {
        return false;
    }

    // Проверка на рокировку
    if (piece.type == PieceType::King && abs(fromX - toX) == 2) {
        return castle(fromX, fromY, toX, toY);
    }

    auto moves = generatePseudoLegalMoves(fromX, fromY);
    if (std::find(moves.begin(), moves.end(), std::make_pair(toX, toY)) == moves.end()) {
        return false;
    }

    // Взятие на проходе
    if (piece.type == PieceType::Pawn && toX != fromX && isEmpty(toX, toY)) {
        grid_[fromY][toX] = {PieceType::None, Color::White, '.'}; // Убираем взятую пешку
    }

    // Делаем ход
    Piece movedPiece = piece;
    if (piece.type == PieceType::Pawn && (toY == 0 || toY == 7)) {
        movedPiece.type = promotion == PieceType::None ? PieceType::Queen : promotion;
    }

    grid_[toY][toX] = movedPiece;
    grid_[fromY][fromX] = {PieceType::None, Color::White, '.'};

    // Обновляем состояние для рокировки
    if (piece.type == PieceType::King) {
        if (piece.color == Color::White) {
            white_king_moved_ = true;
        } else {
            black_king_moved_ = true;
        }
    }

    if (piece.type == PieceType::Rook) {
        if (piece.color == Color::White) {
            if (fromX == 0 && fromY == 7) white_queenside_rook_moved_ = true;
            if (fromX == 7 && fromY == 7) white_kingside_rook_moved_ = true;
        } else {
            if (fromX == 0 && fromY == 0) black_queenside_rook_moved_ = true;
            if (fromX == 7 && fromY == 0) black_kingside_rook_moved_ = true;
        }
    }

    // Проверяем, не остался ли король под шахом
    if (isCheck(current_player_)) {
        // Откатываем ход
        grid_[fromY][fromX] = piece;
        grid_[toY][toX] = {PieceType::None, Color::White, '.'};
        return false;
    }

    current_player_ = (current_player_ == Color::White) ? Color::Black : Color::White;
    return true;
}

bool Board::castle(int kingX, int kingY, int toX, int toY) {
    // Проверяем возможность рокировки
    if (isCheck(current_player_)) return false;

    int direction = (toX > kingX) ? 1 : -1;
    int rookX = (direction > 0) ? 7 : 0;
    
    // Проверяем, что ладья на месте
    if (grid_[kingY][rookX].type != PieceType::Rook || 
        grid_[kingY][rookX].color != current_player_) {
        return false;
    }

    // Проверяем, что король и ладья не двигались
    if (current_player_ == Color::White) {
        if (white_king_moved_) return false;
        if ((direction > 0 && white_kingside_rook_moved_) || 
            (direction < 0 && white_queenside_rook_moved_)) {
            return false;
        }
    } else {
        if (black_king_moved_) return false;
        if ((direction > 0 && black_kingside_rook_moved_) || 
            (direction < 0 && black_queenside_rook_moved_)) {
            return false;
        }
    }

    // Проверяем, что клетки между королем и ладьей свободны
    for (int x = kingX + direction; x != rookX; x += direction) {
        if (!isEmpty(x, kingY)) return false;
    }

    // Проверяем, что король не проходит через атакованные клетки
    for (int x = kingX; x != toX; x += direction) {
        if (isSquareAttacked(x, kingY, current_player_)) return false;
    }

    // Выполняем рокировку
    grid_[kingY][toX] = grid_[kingY][kingX];
    grid_[kingY][kingX] = {PieceType::None, Color::White, '.'};
    
    int rookNewX = (direction > 0) ? toX - 1 : toX + 1;
    grid_[kingY][rookNewX] = grid_[kingY][rookX];
    grid_[kingY][rookX] = {PieceType::None, Color::White, '.'};

    return true;
}

bool Board::isSquareAttacked(int x, int y, Color byColor) const {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (grid_[i][j].type != PieceType::None && grid_[i][j].color == byColor) {
                auto moves = generatePseudoLegalMoves(j, i);
                if (std::find(moves.begin(), moves.end(), std::make_pair(x, y)) != moves.end()) {
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
            if (grid_[y][x].type == PieceType::King && grid_[y][x].color == player) {
                kingX = x;
                kingY = y;
                break;
            }
        }
    }

    if (kingX == -1) return false;
    return isSquareAttacked(kingX, kingY, player == Color::White ? Color::Black : Color::White);
}

bool Board::isCheckmate(Color player) {
    if (!isCheck(player)) return false;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (grid_[y][x].type != PieceType::None && grid_[y][x].color == player) {
                auto moves = generatePseudoLegalMoves(x, y);
                for (const auto& [mx, my] : moves) {
                    // Пробуем сделать ход
                    Piece originalFrom = grid_[y][x];
                    Piece originalTo = grid_[my][mx];
                    
                    grid_[my][mx] = originalFrom;
                    grid_[y][x] = {PieceType::None, Color::White, '.'};
                    
                    bool stillInCheck = isCheck(player);
                    
                    // Откатываем ход
                    grid_[y][x] = originalFrom;
                    grid_[my][mx] = originalTo;
                    
                    if (!stillInCheck) {
                        return false;
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
    
    const Piece& piece = grid_[y][x];
    if (piece.type == PieceType::None || piece.color != current_player_) {
        return validMoves;
    }
    
    for (const auto& [mx, my] : moves) {
        // Создаем временную копию доски
        Board tempBoard = *this;
        
        // Делаем ход на временной доске
        tempBoard.grid_[my][mx] = tempBoard.grid_[y][x];
        tempBoard.grid_[y][x] = {PieceType::None, Color::White, '.'};
        
        // Проверяем, не остался ли король под шахом
        if (!tempBoard.isCheck(current_player_)) {
            validMoves.emplace_back(mx, my);
        }
    }
    
    // Добавляем рокировку, если возможно
    if (piece.type == PieceType::King && !isCheck(current_player_)) {
        // Короткая рокировка
        if (canCastleKingside(current_player_)) {
            validMoves.emplace_back(x + 2, y);
        }
        // Длинная рокировка
        if (canCastleQueenside(current_player_)) {
            validMoves.emplace_back(x - 2, y);
        }
    }
    
    return validMoves;
}

bool Board::canCastleKingside(Color player) const {
    if (player == Color::White) {
        return !white_king_moved_ && !white_kingside_rook_moved_ && 
               isEmpty(5, 7) && isEmpty(6, 7) &&
               !isSquareAttacked(4, 7, Color::Black) &&
               !isSquareAttacked(5, 7, Color::Black) &&
               !isSquareAttacked(6, 7, Color::Black);
    } else {
        return !black_king_moved_ && !black_kingside_rook_moved_ && 
               isEmpty(5, 0) && isEmpty(6, 0) &&
               !isSquareAttacked(4, 0, Color::White) &&
               !isSquareAttacked(5, 0, Color::White) &&
               !isSquareAttacked(6, 0, Color::White);
    }
}

bool Board::canCastleQueenside(Color player) const {
    if (player == Color::White) {
        return !white_king_moved_ && !white_queenside_rook_moved_ && 
               isEmpty(3, 7) && isEmpty(2, 7) && isEmpty(1, 7) &&
               !isSquareAttacked(4, 7, Color::Black) &&
               !isSquareAttacked(3, 7, Color::Black) &&
               !isSquareAttacked(2, 7, Color::Black);
    } else {
        return !black_king_moved_ && !black_queenside_rook_moved_ && 
               isEmpty(3, 0) && isEmpty(2, 0) && isEmpty(1, 0) &&
               !isSquareAttacked(4, 0, Color::White) &&
               !isSquareAttacked(3, 0, Color::White) &&
               !isSquareAttacked(2, 0, Color::White);
    }
}

void Board::highlightMoves(const std::vector<std::pair<int, int>>& moves) {
    clearHighlights();
    for (const auto& [x, y] : moves) {
        grid_[y][x] = {PieceType::Highlight, Color::White, '*'};
    }
}

void Board::clearHighlights() {
    for (auto& row : grid_) {
        for (auto& square : row) {
            if (square.type == PieceType::Highlight) {
                square = {PieceType::None, Color::White, '.'};
            }
        }
    }
}

bool Board::isInBounds(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::isEmpty(int x, int y) const {
    return isInBounds(x, y) && grid_[y][x].type == PieceType::None;
}

bool Board::isEnemy(int x, int y, Color allyColor) const {
    return isInBounds(x, y) && grid_[y][x].type != PieceType::None && grid_[y][x].color != allyColor;
}

std::vector<std::pair<int, int>> Board::generatePseudoLegalMoves(int x, int y) const {
    std::vector<std::pair<int, int>> moves;
    if (!isInBounds(x, y)) return moves;

    const Piece& piece = grid_[y][x];
    if (piece.type == PieceType::None) return moves;

    const Color enemyColor = piece.color == Color::White ? Color::Black : Color::White;

    switch (piece.type) {
        case PieceType::Pawn: {
            int direction = piece.color == Color::White ? -1 : 1;
            int startRow = piece.color == Color::White ? 6 : 1;

            // Ход вперед
            if (isEmpty(x, y + direction)) {
                moves.emplace_back(x, y + direction);
                // Двойной ход с начальной позиции
                if (y == startRow && isEmpty(x, y + 2 * direction)) {
                    moves.emplace_back(x, y + 2 * direction);
                }
            }

            // Взятия
            for (int dx : {-1, 1}) {
                if (isEnemy(x + dx, y + direction, piece.color)) {
                    moves.emplace_back(x + dx, y + direction);
                }
                // Взятие на проходе
                else if (y == (piece.color == Color::White ? 3 : 4) && 
                         isEnemy(x + dx, y, enemyColor) && 
                         grid_[y][x + dx].type == PieceType::Pawn) {
                    moves.emplace_back(x + dx, y + direction);
                }
            }
            break;
        }

        case PieceType::Knight: {
            constexpr std::array<std::pair<int, int>, 8> offsets = {
                {{1, 2}, {2, 1}, {-1, 2}, {-2, 1},
                 {1, -2}, {2, -1}, {-1, -2}, {-2, -1}}
            };

            for (const auto& [dx, dy] : offsets) {
                int nx = x + dx;
                int ny = y + dy;
                if (isInBounds(nx, ny) && (isEmpty(nx, ny) || isEnemy(nx, ny, piece.color))) {
                    moves.emplace_back(nx, ny);
                }
            }
            break;
        }

        case PieceType::Bishop: {
            constexpr std::array<std::pair<int, int>, 4> directions = {
                {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}
            };

            for (const auto& [dx, dy] : directions) {
                for (int step = 1; step < 8; ++step) {
                    int nx = x + dx * step;
                    int ny = y + dy * step;
                    if (!isInBounds(nx, ny)) break;

                    if (isEmpty(nx, ny)) {
                        moves.emplace_back(nx, ny);
                    } else if (isEnemy(nx, ny, piece.color)) {
                        moves.emplace_back(nx, ny);
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case PieceType::Rook: {
            constexpr std::array<std::pair<int, int>, 4> directions = {
                {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
            };

            for (const auto& [dx, dy] : directions) {
                for (int step = 1; step < 8; ++step) {
                    int nx = x + dx * step;
                    int ny = y + dy * step;
                    if (!isInBounds(nx, ny)) break;

                    if (isEmpty(nx, ny)) {
                        moves.emplace_back(nx, ny);
                    } else if (isEnemy(nx, ny, piece.color)) {
                        moves.emplace_back(nx, ny);
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case PieceType::Queen: {
            // Комбинация ходов слона и ладьи
            constexpr std::array<std::pair<int, int>, 8> directions = {
                {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                 {1, 0}, {-1, 0}, {0, 1}, {0, -1}}
            };

            for (const auto& [dx, dy] : directions) {
                for (int step = 1; step < 8; ++step) {
                    int nx = x + dx * step;
                    int ny = y + dy * step;
                    if (!isInBounds(nx, ny)) break;

                    if (isEmpty(nx, ny)) {
                        moves.emplace_back(nx, ny);
                    } else if (isEnemy(nx, ny, piece.color)) {
                        moves.emplace_back(nx, ny);
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case PieceType::King: {
            constexpr std::array<std::pair<int, int>, 8> offsets = {
                {{1, 1}, {1, 0}, {1, -1}, {0, 1},
                 {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}}
            };

            for (const auto& [dx, dy] : offsets) {
                int nx = x + dx;
                int ny = y + dy;
                if (isInBounds(nx, ny) && (isEmpty(nx, ny) || isEnemy(nx, ny, piece.color))) {
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
    // Очищаем доску
    for (auto& row : grid_) {
        for (auto& square : row) {
            square = {PieceType::None, Color::White, '.'};
        }
    }

    // Расставляем пешки
    for (int x = 0; x < 8; ++x) {
        grid_[1][x] = {PieceType::Pawn, Color::Black, 'p'};
        grid_[6][x] = {PieceType::Pawn, Color::White, 'P'};
    }

    // Расставляем черные фигуры
    grid_[0][0] = {PieceType::Rook,   Color::Black, 'r'};
    grid_[0][1] = {PieceType::Knight, Color::Black, 'n'};
    grid_[0][2] = {PieceType::Bishop, Color::Black, 'b'};
    grid_[0][3] = {PieceType::Queen,  Color::Black, 'q'};
    grid_[0][4] = {PieceType::King,   Color::Black, 'k'};
    grid_[0][5] = {PieceType::Bishop, Color::Black, 'b'};
    grid_[0][6] = {PieceType::Knight, Color::Black, 'n'};
    grid_[0][7] = {PieceType::Rook,   Color::Black, 'r'};

    // Расставляем белые фигуры
    grid_[7][0] = {PieceType::Rook,   Color::White, 'R'};
    grid_[7][1] = {PieceType::Knight, Color::White, 'N'};
    grid_[7][2] = {PieceType::Bishop, Color::White, 'B'};
    grid_[7][3] = {PieceType::Queen,  Color::White, 'Q'};
    grid_[7][4] = {PieceType::King,   Color::White, 'K'};
    grid_[7][5] = {PieceType::Bishop, Color::White, 'B'};
    grid_[7][6] = {PieceType::Knight, Color::White, 'N'};
    grid_[7][7] = {PieceType::Rook,   Color::White, 'R'};

    // Сбрасываем флаги рокировки
    white_king_moved_ = false;
    white_kingside_rook_moved_ = false;
    white_queenside_rook_moved_ = false;
    black_king_moved_ = false;
    black_kingside_rook_moved_ = false;
    black_queenside_rook_moved_ = false;
}

} // namespace chess
