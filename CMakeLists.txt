cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

# Настройка C++ стандарта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск необходимых библиотек
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/board.cpp
    src/engine.cpp
    src/sdl_game.cpp
)

# Создание исполняемого файла
add_executable(chess_engine ${SOURCES})

# Подключение библиотек
target_include_directories(chess_engine PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(chess_engine PRIVATE
    SDL2
    SDL2_ttf
)