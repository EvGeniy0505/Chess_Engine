cmake_minimum_required(VERSION 3.10)
project(chess_engine)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(MAIN_SOURCES
    src/engine/computer_player.cpp
    src/engine/move_generator.cpp
    src/engine/position_evaluator.cpp
)

set(BOARD_SOURCES
    ${SOURCE_ROOT}/board/board.cpp
    ${SOURCE_ROOT}/board/initialization.cpp
    ${SOURCE_ROOT}/board/move_generation.cpp
    ${SOURCE_ROOT}/board/castling.cpp
    ${SOURCE_ROOT}/board/check.cpp
)

 set(PIECES_SOURCES
     ${SOURCE_ROOT}/pieces/piece.hpp
     ${SOURCE_ROOT}/pieces/piece_types.hpp
     ${SOURCE_ROOT}/pieces/piece_symbols.hpp
     ${SOURCE_ROOT}/pieces/piece_color.hpp
 )

source_group("Pieces" FILES ${PIECES_SOURCES})

add_executable(chess_engine
    ${SOURCE_ROOT}/main.cpp
    ${MAIN_SOURCES}
    ${BOARD_SOURCES}
    ${PIECES_SOURCES}
)

add_executable(lichess_bot
    ${SOURCE_ROOT}/lichess_bot.cpp
    ${MAIN_SOURCES}
    ${BOARD_SOURCES}
    ${PIECES_SOURCES}
)


target_include_directories(chess_engine PRIVATE
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/pieces
    ${SOURCE_ROOT}/board
    ${SOURCE_ROOT}/engine
)

target_include_directories(lichess_bot PRIVATE
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/pieces
    ${SOURCE_ROOT}/board
    ${SOURCE_ROOT}/engine
)

