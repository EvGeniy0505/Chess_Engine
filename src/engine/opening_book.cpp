#include "engine/opening_book.hpp"
#include "board/initialization.hpp"
#include "pieces/piece.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream> // не забудь добавить, если ещё нет
#include <random>
#include <sstream>

namespace chess::engine {

std::optional<Move> OpeningBook::parseMove(const std::string &moveStr) {
    if (moveStr.size() < 4)
        return std::nullopt;

    auto fileToIndex = [](char file) -> int {
        if (file >= 'a' && file <= 'h')
            return file - 'a';
        return -1;
    };

    auto rankToIndex = [](char rank) -> int {
        if (rank >= '1' && rank <= '8')
            return rank - '1';
        return -1;
    };

    int fromFile = fileToIndex(moveStr[0]);
    int fromRank = rankToIndex(moveStr[1]);
    int toFile = fileToIndex(moveStr[2]);
    int toRank = rankToIndex(moveStr[3]);

    if (fromFile == -1 || fromRank == -1 || toFile == -1 || toRank == -1)
        return std::nullopt;

    return Move{{fromFile, 7 - fromRank}, {toFile, 7 - toRank}};
}

OpeningBook::OpeningBook(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // Можно кидать исключение или логировать ошибку
        return;
    }

    std::string line;
    std::string currentFen;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (line.rfind("pos ", 0) == 0) { // строка начинается с "pos "
            currentFen = line.substr(4);

        } else {
            std::istringstream iss(line);
            std::string moveStr;
            int freq = 0;
            if (iss >> moveStr >> freq) {
                auto moveOpt = parseMove(moveStr);
                if (moveOpt) {
                    book_[currentFen].emplace_back(*moveOpt, freq);
                }
            }
        }
    }
}

std::string replaceEnPassant(std::string fen) {
    if (fen.size() >= 2) {
        // заменим последние два символа на " -"
        fen.replace(fen.size() - 2, 2, "-");
    }
    return fen;
}

std::optional<Move> OpeningBook::getOpeningMove(const Board &board,
                                                Color color) const {
    std::string fen = BoardInitializer::export_to_fen(board);
    // std::string fen = boardToFEN(board, color);
    std::string key = removeMoveCounters(fen);
    key = replaceEnPassant(key);

    // std::cerr << key << '\n';

    auto it = book_.find(key);

    if (it == book_.end() || it->second.empty())
        return std::nullopt;

    const auto &moves = it->second;

    // std::cerr << "OpeningBook: найдено ходов = " << moves.size() << '\n';

    // Параметр топ-N — размер окна, из которого выбираем ход случайно
    const int topN = 5;
    int limit = std::min(topN, static_cast<int>(moves.size()));

    // Копируем топ-N ходов и сортируем по убыванию частоты
    std::vector<std::pair<Move, int>> topMoves(moves.begin(),
                                               moves.begin() + limit);
    std::sort(topMoves.begin(), topMoves.end(),
              [](auto &a, auto &b) { return a.second > b.second; });

    // Выбираем случайный ход из топ-N
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, limit - 1);
    int idx = dist(rng);

    return topMoves[idx].first;
}

std::string OpeningBook::removeMoveCounters(const std::string &fen) {
    // Удаляем последние два поля (halfmove clock и fullmove number)
    size_t lastSpace = fen.rfind(' ');
    if (lastSpace == std::string::npos)
        return fen;
    size_t secondLast = fen.rfind(' ', lastSpace - 1);
    if (secondLast == std::string::npos)
        return fen;
    return fen.substr(0, secondLast);
}

std::string OpeningBook::trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

} // namespace chess::engine
