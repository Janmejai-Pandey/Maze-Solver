#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <random>

#include "Maze.h"
#include "BFSSolver.h"
#include "DFSSolver.h"
#include "Utils.h"

const std::vector<std::string> MAZE_SIMPLE = {
    "###########",
    "S         #",
    "# ####### #",
    "# #       #",
    "# # ##### #",
    "# # #   # #",
    "# # # # # #",
    "# #   #   #",
    "# ####### #",
    "#         E",
    "###########"
};

const std::vector<std::string> MAZE_MEDIUM = {
    "###################",
    "S   #       #     #",
    "### # ##### # ### #",
    "#   #     # # #   #",
    "# ##### ### # # ###",
    "#   #   #   #   # #",
    "### # ### ####### #",
    "#   # #       #   #",
    "# ### # ##### # ###",
    "#   # # #   # #   #",
    "### ### # # # ### #",
    "#       # #     # #",
    "# ####### ####### #",
    "#   #           # #",
    "### # ########### #",
    "#   #             E",
    "###################"
};

const std::vector<std::string> MAZE_TRICKY = {
    "#######################",
    "S #       #       #   #",
    "# # ##### # ##### # # #",
    "#   #   # # #   # # # #",
    "##### # # # # # # ### #",
    "#     # #   # # #     #",
    "# ##### ##### # #######",
    "#   #   #   # #       #",
    "# # # ### # # ####### #",
    "# # #     # #       # #",
    "# # ####### ####### # #",
    "# #         #       # #",
    "# ########### ##### # #",
    "#           #     #   E",
    "#######################"
};

void runSolvers(Maze& maze, const std::string& mazeTitle) {
    Utils::printSeparator(mazeTitle);
    std::cout << "\n" << Color::BOLD << "Original Maze:\n" << Color::RESET;
    maze.display();

    BFSSolver bfs;
    DFSSolver dfs;

    std::cout << "\n" << Color::CYAN << "Running BFS..." << Color::RESET << "\n";
    SolveResult bfsResult = bfs.solve(
        maze.getGrid(), maze.getStart(), maze.getEnd());

    std::cout << Color::YELLOW << "Running DFS..." << Color::RESET << "\n";
    SolveResult dfsResult = dfs.solve(
        maze.getGrid(), maze.getStart(), maze.getEnd());

    maze.displaySolution(bfsResult);
    maze.displaySolution(dfsResult);
    maze.displayComparison(bfsResult, dfsResult);
}

void showAlgorithmInfo() {
    Utils::printSeparator("Algorithm Theory");
    std::cout << Color::BOLD << Color::GREEN
              << "\nBFS (Breadth-First Search):\n" << Color::RESET;
    std::cout << "  Uses a QUEUE (FIFO) - guarantees SHORTEST path\n";
    std::cout << "  Time: O(V+E)  Space: O(V)\n";
    std::cout << Color::BOLD << Color::YELLOW
              << "\nDFS (Depth-First Search):\n" << Color::RESET;
    std::cout << "  Uses a STACK (LIFO) - does NOT guarantee shortest path\n";
    std::cout << "  Time: O(V+E)  Space: O(V)\n";
    std::cout << Color::BOLD << Color::CYAN
              << "\nComparison Table:\n" << Color::RESET;
    std::cout << "  +-----------------------+-----------+-----------+\n";
    std::cout << "  | Property              | BFS       | DFS       |\n";
    std::cout << "  +-----------------------+-----------+-----------+\n";
    std::cout << "  | Data Structure        | Queue     | Stack     |\n";
    std::cout << "  | Shortest Path         | Yes       | No        |\n";
    std::cout << "  | Memory (dense graphs) | More      | Less      |\n";
    std::cout << "  | Exploration Style     | Wide      | Deep      |\n";
    std::cout << "  +-----------------------+-----------+-----------+\n\n";
}

int getValidChoice(int min, int max) {
    int choice;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail() || choice < min || choice > max) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << Color::RED << "Invalid. Enter "
                      << min << "-" << max << ": " << Color::RESET;
        } else {
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
    }
}

int main() {
    Utils::printBanner();
    bool running = true;

    while (running) {
        Utils::printSeparator("Main Menu");
        std::cout << "\n";
        std::cout << "  [1] " << Color::GREEN   << "Solve Simple Maze"
                  << Color::RESET << "\n";
        std::cout << "  [2] " << Color::GREEN   << "Solve Medium Maze"
                  << Color::RESET << "\n";
        std::cout << "  [3] " << Color::GREEN   << "Solve Tricky Maze"
                  << Color::RESET << "\n";
        std::cout << "  [4] " << Color::CYAN    << "Solve Random Generated Maze"
                  << Color::RESET << "\n";
        std::cout << "  [5] " << Color::YELLOW  << "Algorithm Information"
                  << Color::RESET << "\n";
        std::cout << "  [6] " << Color::MAGENTA << "Solve All Preset Mazes"
                  << Color::RESET << "\n";
        std::cout << "  [0] " << Color::RED     << "Exit"
                  << Color::RESET << "\n";
        std::cout << "\n  Choice: ";

        int choice = getValidChoice(0, 6);

        try {
            switch (choice) {
            case 1: {
                Maze m(MAZE_SIMPLE);
                runSolvers(m, "Simple Maze");
                break;
            }
            case 2: {
                Maze m(MAZE_MEDIUM);
                runSolvers(m, "Medium Maze");
                break;
            }
            case 3: {
                Maze m(MAZE_TRICKY);
                runSolvers(m, "Tricky Maze");
                break;
            }
            case 4: {
                std::cout << "\n  Enter seed (0 for random): ";
                unsigned int seed;
                std::cin >> seed;
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');
                if (seed == 0) {
                    std::random_device rd;
                    seed = rd();
                }
                std::cout << "  Enter rows (odd, 11-51): ";
                int rows = getValidChoice(11, 51);
                std::cout << "  Enter cols (odd, 11-51): ";
                int cols = getValidChoice(11, 51);

                Maze m = Maze::generateRandom(rows, cols, seed);
                std::string title = "Random Maze ("
                    + std::to_string(m.getRows()) + "x"
                    + std::to_string(m.getCols()) + ")";
                runSolvers(m, title);
                break;
            }
            case 5:
                showAlgorithmInfo();
                break;
            case 6: {
                Maze m1(MAZE_SIMPLE);
                runSolvers(m1, "Simple Maze");
                Maze m2(MAZE_MEDIUM);
                runSolvers(m2, "Medium Maze");
                Maze m3(MAZE_TRICKY);
                runSolvers(m3, "Tricky Maze");
                break;
            }
            case 0:
                running = false;
                std::cout << Color::CYAN << "\nGoodbye!\n" << Color::RESET;
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << Color::RED << "Error: " << e.what()
                      << Color::RESET << "\n";
        }

        if (running && choice != 5) {
            std::cout << "\nPress Enter to return to menu...";
            std::cin.get();
        }
    }

    return 0;
}