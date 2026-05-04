cmake_minimum_required(VERSION 3.16)
project(MazeSolverGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Find Dependencies ──
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui)

# ── Executable ──
add_executable(maze_solver_gui
    src/main.cpp
    src/App.cpp
    src/Maze.cpp
    src/BFSSolver.cpp
    src/DFSSolver.cpp
    src/MazeRenderer.cpp
    src/ImageExtractor.cpp
    src/Button.cpp
    src/UIPanel.cpp
)

target_include_directories(maze_solver_gui PRIVATE
    src
    ${OpenCV_INCLUDE_DIRS}
)

# 🔥 Modern SFML 3 linking
target_link_libraries(maze_solver_gui
    SFML::Graphics
    SFML::Window
    SFML::System
    ${OpenCV_LIBS}
)

# Copy assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY sample_mazes DESTINATION ${CMAKE_BINARY_DIR})