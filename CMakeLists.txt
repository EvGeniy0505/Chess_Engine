cmake_minimum_required(VERSION 3.10)
project(chess_engine)

set(CMAKE_CXX_STANDARD 17)

add_executable(chess_engine
    src/main.cpp
    src/board.cpp
)
