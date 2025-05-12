cmake_minimum_required(VERSION 3.10)
project(chess_engine)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(MAIN_SOURCES
    ${SOURCE_ROOT}/main.cpp
    src/main.cpp
    src/board.cpp
    src/engine.cpp
    src/ai_move_generator.cpp
)

set(PIECES_SOURCES
    ${SOURCE_ROOT}/pieces/piece.hpp
    ${SOURCE_ROOT}/pieces/pieces_types.hpp
    ${SOURCE_ROOT}/pieces/symbols.hpp
    ${SOURCE_ROOT}/pieces/color.hpp
)

source_group("Pieces" FILES ${PIECES_SOURCES})

add_executable(chess_engine
    ${MAIN_SOURCES}
    ${PIECES_SOURCES}
)

target_include_directories(chess_engine PRIVATE
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/pieces
)

