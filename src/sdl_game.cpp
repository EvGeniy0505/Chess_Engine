#include "sdl_game.hpp"
#include <iostream>
#include <chrono>
#include <thread>

SDLGame::SDLGame(bool vsComputer, chess::Color computerColor) 
    : window(nullptr), renderer(nullptr), font(nullptr), piecesTexture(nullptr),
      isRunning(true), isDragging(false), vsComputer(vsComputer),
      dragStartX(-1), dragStartY(-1), computer(computerColor),
      board(chess::PieceSet::UNICODE) {
    initSDL();
}

SDLGame::~SDLGame() {
    cleanup();
}

void SDLGame::initSDL() {
    // Инициализация SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("SDL_Init Error: " + std::string(SDL_GetError()));
    }

    // Создание окна
    window = SDL_CreateWindow("Chess Game", 
                             SDL_WINDOWPOS_CENTERED, 
                             SDL_WINDOWPOS_CENTERED, 
                             800, 800, 
                             SDL_WINDOW_SHOWN);
    if (!window) {
        throw std::runtime_error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
    }

    // Создание рендерера
    renderer = SDL_CreateRenderer(window, -1, 
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        throw std::runtime_error("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
    }

    // Инициализация SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        throw std::runtime_error("SDL_image could not initialize: " + std::string(IMG_GetError()));
    }

    // Загрузка текстуры с фигурами
    piecesTexture = IMG_LoadTexture(renderer, "../assets/images/chess_pieces.png");
    if (!piecesTexture) {
        std::cerr << "Warning: Failed to load chess pieces texture: " << IMG_GetError() << std::endl;
    }

    // Инициализация SDL_ttf (для текстового фолбэка)
    if (TTF_Init() == -1) {
        std::cerr << "Warning: SDL_ttf could not initialize: " << TTF_GetError() << std::endl;
    }
}

void SDLGame::renderBoard() {
    // Рисуем шахматную доску
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
    if (piece.type == chess::PieceType::None) return;

    // 1. Попытка использовать графические спрайты
    if (piecesTexture) {
        // Размер одной фигуры в спрайте (64x64)
        const int pieceSize = 60;
        SDL_Rect srcRect = {0, 0, pieceSize, pieceSize};

        // Выбираем колонку в зависимости от типа фигуры
        switch (piece.type) {
            case chess::PieceType::Queen:  srcRect.x = 0; break;
            case chess::PieceType::King:   srcRect.x = pieceSize; break;
            case chess::PieceType::Rook:   srcRect.x = pieceSize*2; break;
            case chess::PieceType::Knight: srcRect.x = pieceSize*3; break;
            case chess::PieceType::Bishop: srcRect.x = pieceSize*4; break;
            case chess::PieceType::Pawn:   srcRect.x = pieceSize*5; break;
            default: return;
        }

        
        // Выбираем строку в зависимости от цвета
        srcRect.y = (piece.color == chess::Color::White) ? pieceSize : 0;

        const int drawSize = 100; // Новый размер фигуры
    
        // Центрирование фигуры в клетке
        SDL_Rect destRect = {
            rect.x + (rect.w - drawSize)/2,  // Центр по X
            rect.y + (rect.h - drawSize)/2,  // Центр по Y
            drawSize,                        // Новая ширина
            drawSize                         // Новая высота
        };

        // Рисуем фигуру
        SDL_RenderCopy(renderer, piecesTexture, &srcRect, &destRect);
        return;
    }

    // 2. Fallback: рисуем простые цветные круги
    SDL_Color color = piece.color == chess::Color::White ? 
        SDL_Color{255, 255, 255, 255} : SDL_Color{50, 50, 50, 255};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void SDLGame::renderPieces() {
    // Сначала рисуем все фигуры, кроме перетаскиваемой
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (isDragging && x == dragStartX && y == dragStartY) continue;
            
            const auto& piece = board.getPiece(x, y);
            if (piece.type == chess::PieceType::None) continue;

            SDL_Rect rect = {x * 100 + 18, y * 100 + 18, 64, 64};
            drawPiece(piece, rect);
        }
    }
    
    // Затем рисуем перетаскиваемую фигуру поверх остальных
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
    if (piecesTexture) SDL_DestroyTexture(piecesTexture);
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}