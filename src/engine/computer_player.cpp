#include "engine/computer_player.hpp"
#include "engine/move_generator.hpp"
#include "engine/position_evaluator.hpp"

namespace chess::engine {

ComputerPlayer::ComputerPlayer(Color color,
                               std::unique_ptr<MoveGenerator> generator)
    : color_(color), generator_(std::move(generator)) {}

bool ComputerPlayer::makeMove(Board &board) {
    auto move = generator_->generateBestMove(board, color_);
    return board.make_move(move.from, move.to);
}

std::unique_ptr<ComputerPlayer> ComputerPlayer::create(Color color,
                                                       int difficulty) {
    auto evaluator = std::make_unique<BasicEvaluator>();
    auto generator =
        std::make_unique<MinimaxGenerator>(difficulty, std::move(evaluator));
    return std::make_unique<ComputerPlayer>(color, std::move(generator));
}

} // namespace chess::engine
