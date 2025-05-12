#include "sdl_game.hpp"
#include <iostream>
#include <chrono>
#include <thread>

SDLGame::SDLGame(bool vsComputer, chess::Color computerColor) 
    : window(nullptr), renderer(nullptr), font(nullptr),
      isRunning(true), isDragging(false), vsComputer(vsComputer),
      dragStartX(-1), dragStartY(-1), computer(computerColor),
      board(chess::PieceSet::UNICODE) {
    initSDL();
}

SDLGame::~SDLGame() {
    cleanup();
}

void SDLGame::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("SDL_Init Error: " + std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                             800, 800, SDL_WINDOW_SHOWN);
    if (!window) {
        throw std::runtime_error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        throw std::runtime_error("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
    }

    if (TTF_Init() == -1) {
        throw std::runtime_error("TTF_Init Error: " + std::string(TTF_GetError()));
    }

    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Warning: Failed to load font, using simple rendering. Error: " << TTF_GetError() << std::endl;
    }
}

void SDLGame::renderBoard() {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            SDL_Rect rect = {x * 100, y * 100, 100, 100};
            
            bool isHighlight = false;
            for (const auto& move : possibleMoves) {
                if (move.first == x && move.second == y) {
                    isHighlight = true;
                    break;
                }
            }
            
            if (isHighlight) {
                SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
            } 
            else if ((x + y) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
            } 
            else {
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
            }
            
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void SDLGame::drawPiece(const chess::Piece& piece, const SDL_Rect& rect) {
    SDL_Color color = piece.color == chess::Color::White ? 
        SDL_Color{255, 255, 255, 255} : SDL_Color{0, 0, 0, 255};
    
    if (font) {
        const char* symbol = "?";
        switch (piece.type) {
            case chess::PieceType::Pawn:   symbol = "P"; break;
            case chess::PieceType::Knight: symbol = "N"; break;
            case chess::PieceType::Bishop: symbol = "B"; break;
            case chess::PieceType::Rook:   symbol = "R"; break;
            case chess::PieceType::Queen:  symbol = "Q"; break;
            case chess::PieceType::King:   symbol = "K"; break;
            default: break;
        }
        
        SDL_Surface* surface = TTF_RenderText_Solid(font, symbol, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    } 
    else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void SDLGame::renderPieces() {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (isDragging && x == dragStartX && y == dragStartY) continue;
            
            const auto& piece = board.getPiece(x, y);
            if (piece.type == chess::PieceType::None) continue;

            SDL_Rect rect = {x * 100 + 25, y * 100 + 25, 50, 50};
            drawPiece(piece, rect);
        }
    }
    
    if (isDragging) {
        drawPiece(draggedPiece, dragRect);
    }
}

void SDLGame::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                handleMouseDown(event);
                break;
            case SDL_MOUSEMOTION:
                handleMouseMotion(event);
                break;
            case SDL_MOUSEBUTTONUP:
                handleMouseUp(event);
                break;
        }
    }
}

void SDLGame::handleMouseDown(const SDL_Event& event) {
    if ((!vsComputer || board.current_player_ != computer.getColor()) && 
        event.button.button == SDL_BUTTON_LEFT) {
        
        int mouseX = event.button.x;
        int mouseY = event.button.y;
        dragStartX = mouseX / 100;
        dragStartY = mouseY / 100;
        
        const auto& piece = board.getPiece(dragStartX, dragStartY);
        if (piece.type != chess::PieceType::None && 
            piece.color == board.current_player_) {
            
            isDragging = true;
            draggedPiece = piece;
            dragRect = {mouseX - 25, mouseY - 25, 50, 50};
            possibleMoves = board.getPossibleMoves(dragStartX, dragStartY);
        }
    }
}

void SDLGame::handleMouseMotion(const SDL_Event& event) {
    if (isDragging) {
        dragRect.x = event.motion.x - 25;
        dragRect.y = event.motion.y - 25;
    }
}

void SDLGame::handleMouseUp(const SDL_Event& event) {
    if (isDragging && event.button.button == SDL_BUTTON_LEFT) {
        isDragging = false;
        
        int mouseX = event.button.x;
        int mouseY = event.button.y;
        int targetX = mouseX / 100;
        int targetY = mouseY / 100;
        
        if (board.makeMove(dragStartX, dragStartY, targetX, targetY)) {
            if (vsComputer && board.current_player_ == computer.getColor()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                makeComputerMove();
            }
        }
        
        possibleMoves.clear();
    }
}

void SDLGame::makeComputerMove() {
    if (computer.makeMove(board)) {
        if (board.isCheckmate(computer.getColor() == chess::Color::White ? 
                            chess::Color::Black : chess::Color::White)) {
            std::cout << "Checkmate! Computer wins!" << std::endl;
        }
    }
}

void SDLGame::run() {
    while (isRunning) {
        handleEvents();
        
        if (vsComputer && board.current_player_ == computer.getColor() && !isDragging) {
            makeComputerMove();
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        renderBoard();
        renderPieces();
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void SDLGame::cleanup() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}