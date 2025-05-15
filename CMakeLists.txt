cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENGINE_DEBUG "Enable engine debug output" OFF)

set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE COMMON_SOURCES
    ${SOURCE_ROOT}/pieces/*.cpp
    ${SOURCE_ROOT}/pieces/*.hpp

    ${SOURCE_ROOT}/board/*.cpp
    ${SOURCE_ROOT}/board/*.hpp

    ${SOURCE_ROOT}/engine/*.cpp
    ${SOURCE_ROOT}/engine/*.hpp
)

add_executable(cli_chess
    ${SOURCE_ROOT}/cli_main.cpp
    ${COMMON_SOURCES}
)

if(ENGINE_DEBUG)
    target_compile_definitions(cli_chess PRIVATE ENGINE_DEBUG)
endif()

add_executable(gui_chess
    ${SOURCE_ROOT}/gui_main.cpp
    ${SOURCE_ROOT}/gui/sdl_game.hpp
    ${SOURCE_ROOT}/gui/sdl_game.cpp
    ${COMMON_SOURCES}
)

add_executable(lichess_bot
    ${SOURCE_ROOT}/lichess_bot.cpp
    ${COMMON_SOURCES}
)

foreach(TARGET cli_chess gui_chess lichess_bot)
    target_include_directories(${TARGET} PRIVATE
        ${SOURCE_ROOT}
    )
endforeach()

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

target_include_directories(gui_chess PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(gui_chess PRIVATE
    SDL2
    SDL2_ttf
    SDL2_image
)
