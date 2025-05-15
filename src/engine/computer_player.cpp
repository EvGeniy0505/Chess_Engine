#include "engine/computer_player.hpp"
#include "engine/move_generator.hpp"
#include "engine/position_evaluator.hpp"
#include <iostream> // не забудь добавить, если ещё нет

namespace chess::engine {

ComputerPlayer::ComputerPlayer(Color color,
                               std::unique_ptr<MoveGenerator> generator)
    : color_(color), generator_(std::move(generator)), openingBook_("../assets/opening_book.txt") {}

bool ComputerPlayer::makeMove(Board &board) {
    auto openingMove = openingBook_.getOpeningMove(board, color_);

    if (openingMove) {
        lastMove_ = *openingMove;
    } else {
        lastMove_ = generator_->generateBestMove(board, color_);
    }

    // Выводим ход
    char fromFile = 'a' + lastMove_.from.first;
    char fromRank = '1' + lastMove_.from.second;
    char toFile   = 'a' + lastMove_.to.first;
    char toRank   = '1' + lastMove_.to.second;

    std::cerr << fromFile << fromRank << toFile << toRank << std::endl;


    return board.make_move(lastMove_.from, lastMove_.to);
}

Move ComputerPlayer::getLastMove() const { return lastMove_; }

std::unique_ptr<ComputerPlayer> ComputerPlayer::create(Color color,
                                                       int difficulty) {
    auto evaluator = std::make_unique<PositionEvaluator>();
    auto generator =
        std::make_unique<MinimaxGenerator>(difficulty, std::move(evaluator));
    return std::make_unique<ComputerPlayer>(color, std::move(generator));
}

} // namespace chess::engine
