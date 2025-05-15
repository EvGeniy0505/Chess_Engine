#pragma once
#include "board/board.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

namespace chess::engine {

class DebugLogger {
  public:
    struct ScoredMove {
        Position from;
        Position to;
        float score;

        bool operator<(const ScoredMove &other) const {
            return score > other.score;
        }
    };

    explicit DebugLogger(Color color)
        : color_(color), start_(std::chrono::steady_clock::now()) {
        std::cout << "\n--- Engine Analysis ("
                  << (color == Color::WHITE ? "White" : "Black") << ") ---\n";
    }

    void log_move(Position from, Position to, float score) {
        moves_.push_back({from, to, score / 100.0f});
        nodes_++;
    }

    ~DebugLogger() {
        auto end = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);

        std::sort(moves_.begin(), moves_.end());

        std::cout << "Top moves:\n";
        const size_t count = std::min(size_t(3), moves_.size());
        for (size_t i = 0; i < count; ++i) {
            const auto &m = moves_[i];
            std::cout << i + 1 << ". " << static_cast<char>('a' + m.from.first)
                      << (8 - m.from.second) << " → "
                      << static_cast<char>('a' + m.to.first)
                      << (8 - m.to.second) << " (";

            if (m.score >= 0)
                std::cout << "+";
            std::cout << std::fixed << std::setprecision(2) << m.score << ")\n";
        }

        std::cout << "--------------------------------\n"
                  << "Nodes: " << nodes_ << "\n"
                  << "Time: " << duration.count() << " ms\n"
                  << "--------------------------------\n";
    }

  private:
    Color color_;
    std::vector<ScoredMove> moves_;
    size_t nodes_ = 0;
    std::chrono::steady_clock::time_point start_;
};
} // namespace chess::engine
