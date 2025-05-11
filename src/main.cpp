#include "board.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>

void printHelp() {
    std::cout << "Команды:\n"
              << "help - показать справку\n"
              << "quit - выход\n"
              << "reset - новая игра\n"
              << "show [клетка] - показать ходы для фигуры (например, show e2)\n"
              << "[откуда] [куда] - сделать ход (например, e2 e4)\n"
              << "При превращении пешки добавьте тип фигуры (q, r, b, n): e7 e8 q\n";
}

chess::PieceType getPromotionType(char c) {
    switch (tolower(c)) {
        case 'q': return chess::PieceType::Queen;
        case 'r': return chess::PieceType::Rook;
        case 'b': return chess::PieceType::Bishop;
        case 'n': return chess::PieceType::Knight;
        default: return chess::PieceType::Queen;
    }
}

int main() {
    chess::Board board;
    printHelp();
    board.print();

    while (true) {
        std::string input;
        std::cout << "\nВведите команду или ход: ";
        std::getline(std::cin, input);

        if (input == "help") {
            printHelp();
            continue;
        }
        else if (input == "quit") {
            break;
        }
        else if (input == "reset") {
            board = chess::Board();
            board.print();
            continue;
        }
        else if (input.rfind("show ", 0) == 0) {
            std::string coord = input.substr(5);
            if (coord.length() != 2) {
                std::cerr << "Неверный формат. Пример: show e2\n";
                continue;
            }

            int x = coord[0] - 'a';
            int y = '8' - coord[1];
            
            auto moves = board.getPossibleMoves(x, y);
            if (moves.empty()) {
                std::cout << "Нет возможных ходов для этой фигуры!\n";
                continue;
            }

            std::cout << "Возможные ходы: ";
            for (const auto& [mx, my] : moves) {
                std::cout << char('a' + mx) << (8 - my) << " ";
            }
            std::cout << "\n";

            chess::Board tempBoard = board;
            tempBoard.highlightMoves(moves);
            tempBoard.print(true);
            continue;
        }

        // Обработка хода
        std::istringstream iss(input);
        std::string from, to;
        char promotion = '\0';
        iss >> from >> to >> promotion;

        if (from.size() != 2 || to.size() != 2) {
            std::cerr << "Ошибка: неверный формат ввода. Пример: e2 e4\n";
            continue;
        }

        int fromX = from[0] - 'a';
        int fromY = '8' - from[1];
        int toX = to[0] - 'a';
        int toY = '8' - to[1];
        
        chess::PieceType promoType = chess::PieceType::None;
        if (promotion != '\0') {
            promoType = getPromotionType(promotion);
        }

        if (board.makeMove(fromX, fromY, toX, toY, promoType)) {
            board.print();
            
            if (board.isCheckmate(chess::Color::White)) {
                std::cout << "Мат! Черные победили!\n";
                break;
            }
            if (board.isCheckmate(chess::Color::Black)) {
                std::cout << "Мат! Белые победили!\n";
                break;
            }
        } else {
            std::cerr << "Невозможный ход!\n";
        }
    }

    std::cout << "Игра завершена.\n";
    return 0;
}
