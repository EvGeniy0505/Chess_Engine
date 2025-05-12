#pragma once

namespace chess {

enum class Color { WHITE, BLACK };
enum class CellColor { WHITE, BLACK, HIGHLIGHT_WHITE, HIGHLIGHT_BLACK };

struct ColorCodes {
    const char *background;
    const char *foreground_white;
    const char *foreground_black;
};

inline const ColorCodes &getColorCodes(CellColor cellColor) {
    static constexpr ColorCodes whiteCell = {"48;5;237", "38;5;15", "38;5;250"};
    static constexpr ColorCodes blackCell = {"48;5;236", "38;5;15", "38;5;245"};
    static constexpr ColorCodes highlightCell = {"48;5;238", "38;5;15",
                                                 "38;5;250"};

    switch (cellColor) {
        case CellColor::WHITE:
            return whiteCell;
        case CellColor::BLACK:
            return blackCell;
        default:
            return highlightCell;
    }
}

} // namespace chess
