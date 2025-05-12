#include "sdl_game.hpp"
#include <iostream>

SDLGame::SDLGame() : window(nullptr), renderer(nullptr), font(nullptr), isRunning(true) {
    initSDL();
}

void SDLGame::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                             800, 800, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        // Продолжаем без шрифта, будем использовать примитивы
    }
}

void SDLGame::renderBoard() {
    // Рисуем доску
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            SDL_Rect rect = {x * 100, y * 100, 100, 100};
            
            // Подсветка возможных ходов
            bool isHighlight = false;
            for (const auto& move : possibleMoves) {
                if (move.first == x && move.second == y) {
                    isHighlight = true;
                    break;
                }
            }
            
            if (isHighlight) {
                SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255); // Зеленая подсветка
            } else if ((x + y) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Светлые клетки
            } else {
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);  // Темные клетки
            }
            
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void SDLGame::renderPieces() {
    // Сначала рисуем все фигуры, кроме перетаскиваемой
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            // Не рисуем фигуру, которую перетаскиваем
            if (isDragging && x == dragStartX && y == dragStartY) continue;
            
            const auto& piece = board.getPiece(x, y);
            if (piece.type == chess::PieceType::None) continue;

            SDL_Rect rect = {x * 100 + 25, y * 100 + 25, 50, 50};
            drawPiece(piece, rect);
        }
    }
    
    // Затем рисуем перетаскиваемую фигуру поверх остальных
    if (isDragging) {
        drawPiece(draggedPiece, dragRect);
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
    } else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
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
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    dragStartX = mouseX / 100;
                    dragStartY = mouseY / 100;
                    
                    // Проверяем, есть ли фигура в этой клетке
                    const auto& piece = board.getPiece(dragStartX, dragStartY);
                    if (piece.type != chess::PieceType::None && piece.color == board.current_player_) {
                        isDragging = true;
                        draggedPiece = piece;
                        dragRect.x = mouseX - 50; // Центрируем на курсоре
                        dragRect.y = mouseY - 50;
                        possibleMoves = board.getPossibleMoves(dragStartX, dragStartY);
                    }
                }
                break;
                
            case SDL_MOUSEMOTION:
                if (isDragging) {
                    dragRect.x = event.motion.x - 50;
                    dragRect.y = event.motion.y - 50;
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (isDragging && event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    int targetX = mouseX / 100;
                    int targetY = mouseY / 100;
                    
                    // Пытаемся сделать ход
                    if (board.makeMove(dragStartX, dragStartY, targetX, targetY)) {
                        // Ход успешен
                    }
                    
                    // Сбрасываем состояние перетаскивания
                    isDragging = false;
                    dragStartX = -1;
                    dragStartY = -1;
                    possibleMoves.clear();
                }
                break;
        }
    }
}

void SDLGame::run() {
    while (isRunning) {
        handleEvents();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        renderBoard();
        renderPieces();
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }
}

SDLGame::~SDLGame() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}