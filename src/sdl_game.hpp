#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "board.hpp"
#include "pieces.hpp"  // Добавляем этот include

class SDLGame {
public:
    SDLGame();
    ~SDLGame();
    void run();

private:
    void initSDL();
    void renderBoard();
    void renderPieces();
    void drawPiece(const chess::Piece& piece, const SDL_Rect& rect);  // Добавляем объявление
    void handleEvents();
    void cleanup();

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    chess::Board board;
    bool isRunning;
    bool isDragging;
    int dragStartX, dragStartY;
    SDL_Rect dragRect;
    chess::Piece draggedPiece;  // Используем полное квалифицированное имя
    std::vector<std::pair<int, int>> possibleMoves;
};