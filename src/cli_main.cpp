#include "board/board.hpp"
#include "engine/computer_player.hpp"
#include "engine/move_generator.hpp" // Добавляем этот include
#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory> // Для std::make_unique
#include <sstream>
#include <string>

void printHelp() {
    std::cout
        << "Использование: chess_engine [--piece_type TYPE] [--computer]\n"
        << "Доступные типы фигур:\n"
        << "  unicode  - Unicode символы (по умолчанию)\n"
        << "  letters  - Буквенные обозначения (K, Q, R и т.д.)\n"
        << "Опции:\n"
        << "  --computer - игра против компьютера (компьютер играет чёрными)\n"
        << "Команды во время игры:\n"
        << "  help h     - показать справку\n"
        << "  quit q     - выход\n"
        << "  reset r    - новая игра\n"
        << "  show [клетка] - показать ходы для фигуры (например, show e2)\n"
        << "  [откуда] [куда] - сделать ход (например, e2 e4)\n"
        << "  При превращении пешки добавьте тип фигуры (q, r, b, n): e7 e8 q\n"
        << "  Для выхода введите 'exit'\n";
}

chess::PieceType getPromotionType(char c) {
    switch (tolower(c)) {
        case 'q':
            return chess::PieceType::QUEEN;
        case 'r':
            return chess::PieceType::ROOK;
        case 'b':
            return chess::PieceType::BISHOP;
        case 'n':
            return chess::PieceType::KNIGHT;
        default:
            return chess::PieceType::QUEEN;
    }
}

chess::PieceSet parsePieceSet(const std::string &type) {
    if (type == "unicode")
        return chess::PieceSet::UNICODE;
    if (type == "letters")
        return chess::PieceSet::LETTERS;
    return chess::PieceSet::UNICODE;
}

int main(int argc, char *argv[]) {
    chess::PieceSet pieceSet = chess::PieceSet::UNICODE;
    bool vsComputer = false;

    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--piece_type" && i + 1 < argc) {
            std::string type = argv[++i];
            std::transform(type.begin(), type.end(), type.begin(), ::tolower);
            pieceSet = parsePieceSet(type);
        } else if (arg == "--computer") {
            vsComputer = true;
        } else if (arg == "--help") {
            printHelp();
            return 0;
        } else {
            std::cerr << "Неизвестный аргумент: " << arg << "\n";
            printHelp();
            return 1;
        }
    }

// Для Windows: включаем поддержку UTF-8 в консоли
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    chess::Board board;

    // Создаём компьютерного игрока с генератором ходов
    auto computer =
        chess::engine::ComputerPlayer::create(chess::Color::BLACK, 3);

    printHelp();
    board.print();

    while (true) {
        // Ход человека
        if (!vsComputer || board.current_player == chess::Color::WHITE) {
            std::string input;
            std::cout << "\nВведите команду или ход: ";
            std::getline(std::cin, input);

            if (input == "help" || input == "h") {
                printHelp();
                continue;
            } else if (input == "quit" || input == "q") {
                break;
            } else if (input == "reset" || input == "r") {
                board = chess::Board();
                board.print();
                continue;
            } else if (input.rfind("show ", 0) == 0) {
                std::string coord = input.substr(5);
                if (coord.length() != 2) {
                    std::cerr << "Неверный формат. Пример: show e2\n";
                    continue;
                }

                int x = coord[0] - 'a';
                int y = '8' - coord[1];

                auto moves = board.get_legal_moves({x, y});
                if (moves.empty()) {
                    std::cout << "Нет возможных ходов для этой фигуры!\n";
                    continue;
                }

                std::cout << "Возможные ходы: ";
                for (const auto &[mx, my] : moves) {
                    std::cout << char('a' + mx) << (8 - my) << " ";
                }
                std::cout << "\n";

                chess::Board tempBoard = board;
                tempBoard.highlight_moves(moves);
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

            chess::PieceType promoType = chess::PieceType::NONE;
            if (promotion != '\0') {
                promoType = getPromotionType(promotion);
            }

            if (board.make_move({fromX, fromY}, {toX, toY}, promoType)) {
                board.print();

                if (board.is_checkmate(chess::Color::BLACK)) {
                    std::cout << "Мат! Белые победили!\n";
                    break;
                }
            } else {
                std::cerr << "Невозможный ход!\n";
            }
        }
        // Ход компьютера
        else {
            std::cout << "\nХод компьютера...\n";
            // std::this_thread::sleep_for(std::chrono::seconds(1));

            if (!computer->makeMove(board)) {
                std::cout << "У компьютера нет возможных ходов!\n";
                break;
            }

            board.print();

            if (board.is_checkmate(chess::Color::WHITE)) {
                std::cout << "Мат! Чёрные победили!\n";
                break;
            }
        }
    }

    std::cout << "Игра завершена.\n";
    return 0;
}
