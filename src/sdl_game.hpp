#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "board.hpp"
#include "pieces.hpp"
#include "engine.hpp"

class SDLGame {
public:
    SDLGame(bool vsComputer = false, chess::Color computerColor = chess::Color::Black);
    ~SDLGame();
    void run();

private:
    void initSDL();
    void renderBoard();
    void renderPieces();
    void drawPiece(const chess::Piece& piece, const SDL_Rect& rect);
    void handleEvents();
    void handleMouseDown(const SDL_Event& event);
    void handleMouseMotion(const SDL_Event& event);
    void handleMouseUp(const SDL_Event& event);
    void makeComputerMove();
    void cleanup();

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    chess::Board board;
    chess::ComputerPlayer computer;
    bool isRunning;
    bool isDragging;
    bool vsComputer;
    int dragStartX, dragStartY;
    SDL_Rect dragRect;
    chess::Piece draggedPiece;
    std::vector<std::pair<int, int>> possibleMoves;
};