cmake_minimum_required(VERSION 3.14)
project(MazeSolver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-std=c++17)

# Enable warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

add_executable(maze_solver
    src/main.cpp
    src/Maze.cpp
    src/BFSSolver.cpp
    src/DFSSolver.cpp
)

target_include_directories(maze_solver PRIVATE src)