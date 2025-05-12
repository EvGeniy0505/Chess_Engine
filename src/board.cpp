#include "board.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

namespace chess {

Board::Board(PieceSet set) : piece_set_(set) { initializeBoard(); }

void Board::print(bool showHighlights) {
  // Верхняя координатная сетка
  std::cout << "\n   a  b  c  d  e  f  g  h\n";

  for (int y = 0; y < 8; ++y) {
    // Номер строки слева
    std::cout << 8 - y << " ";

    for (int x = 0; x < 8; ++x) {
      Piece &piece = grid_[y][x];

      // Устанавливаем цвет клетки
      piece.cell_color = ((x + y) % 2) ? CellColor::BLACK : CellColor::WHITE;

      // Подсветка ходов
      if (showHighlights && piece.type == PieceType::Highlight) {
        piece.cell_color = ((x + y) % 2) ? CellColor::HIGHLIGHT_BLACK
                                         : CellColor::HIGHLIGHT_WHITE;
      }

      // Вывод фигуры с цветом
      std::cout << piece.getColoredSymbol(piece_set_);
    }

    // Номер строки справа
    std::cout << " " << 8 - y << "\n";
  }

  // Нижняя координатная сетка
  std::cout << "   a  b  c  d  e  f  g  h\n\n";

  // Дополнительная информация
  std::cout << "Текущий игрок: "
            << (current_player_ == Color::White ? "Белые" : "Чёрные") << "\n";
  if (isCheck(current_player_)) {
    std::cout << "ШАХ!\n";
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

  // Сохраняем исходные фигуры перед ходом
  Piece originalFrom = piece;
  Piece originalTo = grid_[toY][toX];

  // Специальная обработка рокировки
  if (piece.type == PieceType::King && abs(fromX - toX) == 2) {
      return castle(fromX, fromY, toX, toY);
  }

  auto moves = generatePseudoLegalMoves(fromX, fromY);
  if (std::find(moves.begin(), moves.end(), std::make_pair(toX, toY)) == moves.end()) {
      return false;
  }

  // Взятие на проходе
  if (piece.type == PieceType::Pawn && toX != fromX && isEmpty(toX, toY)) {
      grid_[fromY][toX] = Piece(PieceType::None, Color::White, ".");
  }

  // Выполняем ход
  Piece movedPiece = piece;
  if (piece.type == PieceType::Pawn && (toY == 0 || toY == 7)) {
      movedPiece.type = promotion == PieceType::None ? PieceType::Queen : promotion;
      movedPiece.display = getPieceSymbol(movedPiece.type, movedPiece.color);
  }

  // Сохраняем состояние рокировки перед изменением
  bool originalWhiteKingMoved = white_king_moved_;
  bool originalBlackKingMoved = black_king_moved_;
  bool originalWhiteKingsideRook = white_kingside_rook_moved_;
  bool originalWhiteQueensideRook = white_queenside_rook_moved_;
  bool originalBlackKingsideRook = black_kingside_rook_moved_;
  bool originalBlackQueensideRook = black_queenside_rook_moved_;

  grid_[toY][toX] = movedPiece;
  grid_[fromY][fromX] = Piece(PieceType::None, Color::White, ".");
  
  updateCastlingState(fromX, fromY);

  // Проверяем, не остался ли король под шахом
  if (isCheck(current_player_)) {
      // Откатываем ход полностью
      grid_[fromY][fromX] = originalFrom;
      grid_[toY][toX] = originalTo;
      
      // Восстанавливаем состояние рокировки
      white_king_moved_ = originalWhiteKingMoved;
      black_king_moved_ = originalBlackKingMoved;
      white_kingside_rook_moved_ = originalWhiteKingsideRook;
      white_queenside_rook_moved_ = originalWhiteQueensideRook;
      black_kingside_rook_moved_ = originalBlackKingsideRook;
      black_queenside_rook_moved_ = originalBlackQueensideRook;
      
      return false;
  }

  current_player_ = (current_player_ == Color::White) ? Color::Black : Color::White;
  return true;
}

bool Board::castle(int kingX, int kingY, int toX, int toY) {
  if (isCheck(current_player_))
    return false;

  int direction = (toX > kingX) ? 1 : -1;
  int rookX = (direction > 0) ? 7 : 0;

  if (grid_[kingY][rookX].type != PieceType::Rook ||
      grid_[kingY][rookX].color != current_player_) {
    return false;
  }

  for (int x = kingX + direction; x != rookX; x += direction) {
    if (!isEmpty(x, kingY))
      return false;
  }

  for (int x = kingX; x != toX + direction; x += direction) {
    if (isSquareAttacked(x, kingY,
                         current_player_ == Color::White ? Color::Black
                                                         : Color::White)) {
      return false;
    }
  }

  grid_[kingY][toX] = grid_[kingY][kingX];
  grid_[kingY][kingX] = Piece(PieceType::None, Color::White, ".");

  int rookNewX = (direction > 0) ? toX - 1 : toX + 1;
  grid_[kingY][rookNewX] = grid_[kingY][rookX];
  grid_[kingY][rookX] = Piece(PieceType::None, Color::White, ".");

  if (current_player_ == Color::White) {
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

  current_player_ =
      (current_player_ == Color::White) ? Color::Black : Color::White;

  return true;
}

void Board::updateCastlingState(int x, int y) {
  Piece &piece = grid_[y][x];
  if (piece.type == PieceType::King) {
    if (piece.color == Color::White)
      white_king_moved_ = true;
    else
      black_king_moved_ = true;
  } else if (piece.type == PieceType::Rook) {
    if (piece.color == Color::White) {
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
      if (grid_[i][j].type != PieceType::None && grid_[i][j].color == byColor) {
        auto moves = generatePseudoLegalMoves(j, i);
        if (std::find(moves.begin(), moves.end(), std::make_pair(x, y)) !=
            moves.end()) {
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

  if (kingX == -1)
    return false;
  return isSquareAttacked(kingX, kingY,
                          player == Color::White ? Color::Black : Color::White);
}

bool Board::isCheckmate(Color player) {
  if (!isCheck(player))
    return false;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      if (grid_[y][x].type != PieceType::None && grid_[y][x].color == player) {
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
  if (piece.type == PieceType::None || piece.color != current_player_) {
    return validMoves;
  }

  for (const auto &[mx, my] : moves) {
    Board tempBoard = *this;
    if (tempBoard.makeMove(x, y, mx, my)) {
      validMoves.emplace_back(mx, my);
    }
  }

  if (piece.type == PieceType::King && !isCheck(current_player_)) {
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
  if (player == Color::White) {
    return !white_king_moved_ && !white_kingside_rook_moved_ && isEmpty(5, 7) &&
           isEmpty(6, 7) && !isSquareAttacked(4, 7, Color::Black) &&
           !isSquareAttacked(5, 7, Color::Black);
  } else {
    return !black_king_moved_ && !black_kingside_rook_moved_ && isEmpty(5, 0) &&
           isEmpty(6, 0) && !isSquareAttacked(4, 0, Color::White) &&
           !isSquareAttacked(5, 0, Color::White);
  }
}

bool Board::canCastleQueenside(Color player) const {
  if (player == Color::White) {
    return !white_king_moved_ && !white_queenside_rook_moved_ &&
           isEmpty(3, 7) && isEmpty(2, 7) && isEmpty(1, 7) &&
           !isSquareAttacked(4, 7, Color::Black) &&
           !isSquareAttacked(3, 7, Color::Black);
  } else {
    return !black_king_moved_ && !black_queenside_rook_moved_ &&
           isEmpty(3, 0) && isEmpty(2, 0) && isEmpty(1, 0) &&
           !isSquareAttacked(4, 0, Color::White) &&
           !isSquareAttacked(3, 0, Color::White);
  }
}

void Board::highlightMoves(const std::vector<std::pair<int, int>> &moves) {
  clearHighlights();
  for (const auto &[x, y] : moves) {
    grid_[y][x] = Piece(PieceType::Highlight, Color::White, "*");
  }
}

void Board::clearHighlights() {
  for (auto &row : grid_) {
    for (auto &square : row) {
      if (square.type == PieceType::Highlight) {
        square = Piece(PieceType::None, Color::White, ".");
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
  return isInBounds(x, y) && grid_[y][x].type != PieceType::None &&
         grid_[y][x].color != allyColor;
}

std::string Board::getPieceSymbol(PieceType type, Color color) const {
  static const std::string symbols[2][6] = {
      {"♙", "♘", "♗", "♖", "♕", "♔"}, // Белые
      {"♟", "♞", "♝", "♜", "♛", "♚"}  // Чёрные
  };

  if (type == PieceType::None)
    return ".";
  if (type == PieceType::Highlight)
    return "*";

  int idx = static_cast<int>(type) - 1;
  return symbols[color == Color::White ? 0 : 1][idx];
}

std::vector<std::pair<int, int>> Board::generatePseudoLegalMoves(int x,
                                                                 int y) const {
  std::vector<std::pair<int, int>> moves;
  if (!isInBounds(x, y))
    return moves;

  const Piece &piece = grid_[y][x];
  if (piece.type == PieceType::None)
    return moves;

  const Color enemyColor =
      piece.color == Color::White ? Color::Black : Color::White;
  const int startRow = piece.color == Color::White ? 6 : 1;
  const int enPassantRow = piece.color == Color::White ? 3 : 4;

  switch (piece.type) {
  case PieceType::Pawn: {
    const int direction = piece.color == Color::White ? -1 : 1;

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

      if (isEnemy(x + dx, y + direction, piece.color)) {
        moves.emplace_back(x + dx, y + direction);
      }

      if (y == enPassantRow) {
        if (isEnemy(x + dx, y, piece.color) &&
            grid_[y][x + dx].type == PieceType::Pawn) {
          moves.emplace_back(x + dx, y + direction);
        }
      }
    }
    break;
  }

  case PieceType::Knight: {
    constexpr std::array<std::pair<int, int>, 8> offsets = {{{1, 2},
                                                             {2, 1},
                                                             {-1, 2},
                                                             {-2, 1},
                                                             {1, -2},
                                                             {2, -1},
                                                             {-1, -2},
                                                             {-2, -1}}};

    for (const auto &[dx, dy] : offsets) {
      int nx = x + dx;
      int ny = y + dy;
      if (isInBounds(nx, ny) &&
          (isEmpty(nx, ny) || isEnemy(nx, ny, piece.color))) {
        moves.emplace_back(nx, ny);
      }
    }
    break;
  }

  case PieceType::Bishop: {
    constexpr std::array<std::pair<int, int>, 4> directions = {
        {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

    for (const auto &[dx, dy] : directions) {
      for (int step = 1; step < 8; ++step) {
        int nx = x + dx * step;
        int ny = y + dy * step;
        if (!isInBounds(nx, ny))
          break;

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
        {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

    for (const auto &[dx, dy] : directions) {
      for (int step = 1; step < 8; ++step) {
        int nx = x + dx * step;
        int ny = y + dy * step;
        if (!isInBounds(nx, ny))
          break;

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
    constexpr std::array<std::pair<int, int>, 8> directions = {
        {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

    for (const auto &[dx, dy] : directions) {
      for (int step = 1; step < 8; ++step) {
        int nx = x + dx * step;
        int ny = y + dy * step;
        if (!isInBounds(nx, ny))
          break;

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
        {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}}};

    for (const auto &[dx, dy] : offsets) {
      int nx = x + dx;
      int ny = y + dy;
      if (isInBounds(nx, ny) &&
          (isEmpty(nx, ny) || isEnemy(nx, ny, piece.color))) {
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
  for (auto &row : grid_) {
    for (auto &square : row) {
      square = Piece(PieceType::None, Color::White, ".");
    }
  }

  for (int x = 0; x < 8; ++x) {
    grid_[1][x] = {PieceType::Pawn, Color::Black,
                   getPieceSymbol(PieceType::Pawn, Color::Black)};
    grid_[6][x] = {PieceType::Pawn, Color::White,
                   getPieceSymbol(PieceType::Pawn, Color::White)};
  }

  grid_[0][0] = {PieceType::Rook, Color::Black,
                 getPieceSymbol(PieceType::Rook, Color::Black)};
  grid_[0][1] = {PieceType::Knight, Color::Black,
                 getPieceSymbol(PieceType::Knight, Color::Black)};
  grid_[0][2] = {PieceType::Bishop, Color::Black,
                 getPieceSymbol(PieceType::Bishop, Color::Black)};
  grid_[0][3] = {PieceType::Queen, Color::Black,
                 getPieceSymbol(PieceType::Queen, Color::Black)};
  grid_[0][4] = {PieceType::King, Color::Black,
                 getPieceSymbol(PieceType::King, Color::Black)};
  grid_[0][5] = {PieceType::Bishop, Color::Black,
                 getPieceSymbol(PieceType::Bishop, Color::Black)};
  grid_[0][6] = {PieceType::Knight, Color::Black,
                 getPieceSymbol(PieceType::Knight, Color::Black)};
  grid_[0][7] = {PieceType::Rook, Color::Black,
                 getPieceSymbol(PieceType::Rook, Color::Black)};

  grid_[7][0] = {PieceType::Rook, Color::White,
                 getPieceSymbol(PieceType::Rook, Color::White)};
  grid_[7][1] = {PieceType::Knight, Color::White,
                 getPieceSymbol(PieceType::Knight, Color::White)};
  grid_[7][2] = {PieceType::Bishop, Color::White,
                 getPieceSymbol(PieceType::Bishop, Color::White)};
  grid_[7][3] = {PieceType::Queen, Color::White,
                 getPieceSymbol(PieceType::Queen, Color::White)};
  grid_[7][4] = {PieceType::King, Color::White,
                 getPieceSymbol(PieceType::King, Color::White)};
  grid_[7][5] = {PieceType::Bishop, Color::White,
                 getPieceSymbol(PieceType::Bishop, Color::White)};
  grid_[7][6] = {PieceType::Knight, Color::White,
                 getPieceSymbol(PieceType::Knight, Color::White)};
  grid_[7][7] = {PieceType::Rook, Color::White,
                 getPieceSymbol(PieceType::Rook, Color::White)};

  white_king_moved_ = false;
  white_kingside_rook_moved_ = false;
  white_queenside_rook_moved_ = false;
  black_king_moved_ = false;
  black_kingside_rook_moved_ = false;
  black_queenside_rook_moved_ = false;
}

} // namespace chess
