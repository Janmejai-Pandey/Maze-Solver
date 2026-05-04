cmake_minimum_required(VERSION 3.16)
project(MazeSolverGUI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 3 uses capitalized component names
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(maze_solver_gui
    src/main.cpp
    src/App.cpp
    src/Maze.cpp
    src/BFSSolver.cpp
    src/DFSSolver.cpp
    src/MazeRenderer.cpp
    src/Button.cpp
    src/UIPanel.cpp
)

target_include_directories(maze_solver_gui PRIVATE src)

target_link_libraries(maze_solver_gui
    SFML::Graphics
    SFML::Window
    SFML::System
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})