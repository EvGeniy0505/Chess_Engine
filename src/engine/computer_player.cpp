#include "engine/computer_player.hpp"
#include "engine/move_generator.hpp"
#include "engine/position_evaluator.hpp"
#include <iostream>

namespace chess::engine {

ComputerPlayer::ComputerPlayer(Color color,
                               std::unique_ptr<MoveGenerator> generator)
    : color_(color), generator_(std::move(generator)),
      openingBook_("../assets/opening_book.txt") {}

bool ComputerPlayer::makeMove(Board &board) {
    auto openingMove = openingBook_.getOpeningMove(board, color_);

    if (openingMove) {
        lastMove_ = *openingMove;
    } else {
        lastMove_ = generator_->generateBestMove(board, color_);
    }

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
