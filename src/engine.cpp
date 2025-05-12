#include "engine.hpp"

namespace chess {

bool ComputerPlayer::makeMove(Board& board) {
    std::vector<std::tuple<int, int, int, int>> possibleMoves;

    // Собираем все возможные ходы
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (board.getPiece(x, y).color == color_) {
                auto moves = board.getPossibleMoves(x, y);
                for (const auto& [toX, toY] : moves) {
                    possibleMoves.emplace_back(x, y, toX, toY);
                }
            }
        }
    }

    if (possibleMoves.empty()) {
        return false;
    }

    // Выбираем случайный ход
    std::uniform_int_distribution<> dist(0, possibleMoves.size() - 1);
    auto [fromX, fromY, toX, toY] = possibleMoves[dist(gen_)];
    return board.makeMove(fromX, fromY, toX, toY);
}

} // namespace chess