#include "board/board.hpp"
#include "engine/computer_player.hpp"
#include "engine/move_generator.hpp"
#include "pieces/piece.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using namespace std;

class EngineUCI {
  private:
    chess::Board board;
    unique_ptr<chess::engine::ComputerPlayer> computer;
    bool isBotTurn = false;
    chess::Color botColor; // Храним цвет, за который играет бот

  public:
    EngineUCI()
        : botColor(chess::Color::BLACK) { // По умолчанию бот играет чёрными
        initializeComputerPlayer(botColor);
    }

    void receiveCommand(const string &message) {
        string messageType = message.substr(0, message.find(' '));

        if (messageType == "uci") {
            respond("id name ChessEngine");
            respond("id author YourName");
            respond("uciok");
        } else if (messageType == "isready") {
            respond("readyok");
        } else if (messageType == "ucinewgame") {
            board = chess::Board();
            // При новой игре бот остаётся играть тем же цветом
        } else if (messageType == "position") {
            processPositionCommand(message);
        } else if (messageType == "go") {
            isBotTurn = true;
            processGoCommand(message);
        } else if (messageType == "quit") {
            exit(0);
        } else {
            cerr << "Unrecognized command: " << messageType << endl;
        }
    }

  private:
    void respond(const string &response) { cout << response << endl; }

    void initializeComputerPlayer(chess::Color color) {
        computer = chess::engine::ComputerPlayer::create(color, 3);
        botColor = color;
    }

    void processPositionCommand(const string &message) {
        size_t startpos = message.find("startpos");
        if (startpos != string::npos) {
            board = chess::Board();
        } else {
            size_t fenpos = message.find("fen ");
            if (fenpos != string::npos) {
                string fen = message.substr(fenpos + 4,
                                            message.find("moves") - fenpos - 4);
                board = chess::Board(fen);
            }
        }

        size_t movespos = message.find("moves");
        if (movespos != string::npos) {
            string moves = message.substr(movespos + 5);
            istringstream iss(moves);
            string move;
            while (iss >> move) {
                if (!processMove(move)) {
                    cerr << "Illegal move: " << move << endl;
                    return;
                }
            }
        }

        // Не меняем цвет бота здесь - он определяется при инициализации
    }

    bool processMove(const string &moveStr) {
        if (moveStr.length() < 4)
            return false;

        int fromX = moveStr[0] - 'a';
        int fromY = '8' - moveStr[1];
        int toX = moveStr[2] - 'a';
        int toY = '8' - moveStr[3];

        // Проверяем легальность хода перед выполнением
        auto legalMoves = board.get_legal_moves({fromX, fromY});
        bool isLegal = false;
        for (const auto &m : legalMoves) {
            if (m.first == toX && m.second == toY) {
                isLegal = true;
                break;
            }
        }

        if (!isLegal)
            return false;

        return board.make_move({fromX, fromY}, {toX, toY});
    }

    void processGoCommand(const string &message) {
        if (!isBotTurn || board.current_player != botColor) {
            // Бот ходит только когда его очередь и цвет совпадает
            return;
        }

        if (computer->makeMove(board)) {
            chess::engine::Move move = computer->getLastMove();
            string bestmove = string(1, 'a' + move.from.first) +
                              to_string(8 - move.from.second) +
                              string(1, 'a' + move.to.first) +
                              to_string(8 - move.to.second);

            respond("bestmove " + bestmove);
        } else {
            // Если нет возможных ходов (мат или пат)
            respond("bestmove 0000");
        }
        isBotTurn = false;
    }
};

int main() {
    EngineUCI engine;
    string line;

    while (getline(cin, line)) {
        engine.receiveCommand(line);
    }

    return 0;
}
