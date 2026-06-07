cmake_minimum_required(VERSION 3.10)
project(connect4-cli)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(connect4
    apps/main.cpp
    src/Board.cpp
    src/GameEngine.cpp
    src/HumanPlayer.cpp
    src/AIPlayer.cpp
)
