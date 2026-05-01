#include "Maze.h"
#include "Utils.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <stack>

Maze::Maze(const std::vector<std::string>& lines) {
    if (lines.empty()) throw std::runtime_error("Maze cannot be empty");
    rows_ = (int)lines.size();
    cols_ = (int)lines[0].size();
    grid_.resize(rows_, std::vector<char>(cols_, '#'));
    for (int r = 0; r < rows_; r++)
        for (int c = 0; c < std::min(cols_, (int)lines[r].size()); c++)
            grid_[r][c] = lines[r][c];
    parseStartEnd();
}

void Maze::parseStartEnd() {
    bool foundS = false, foundE = false;
    for (int r = 0; r < rows_; r++) {
        for (int c = 0; c < cols_; c++) {
            if (grid_[r][c] == 'S') { start_ = {r, c}; foundS = true; }
            if (grid_[r][c] == 'E') { end_   = {r, c}; foundE = true; }
        }
    }
    if (!foundS || !foundE)
        throw std::runtime_error("Maze must contain 'S' and 'E'");
}

Maze Maze::generateRandom(int rows, int cols, unsigned int seed) {
    if (rows % 2 == 0) rows++;
    if (cols % 2 == 0) cols++;
    std::vector<std::string> lines(rows, std::string(cols, '#'));
    std::mt19937 rng(seed);

    auto carve = [&](int r, int c) {
        struct Frame { int r, c; };
        std::stack<Frame> stk;
        stk.push({r, c});
        lines[r][c] = ' ';
        const int dr[] = {-2, 2, 0, 0};
        const int dc[] = { 0, 0, -2, 2};
        while (!stk.empty()) {
            auto [cr, cc] = stk.top();
            std::vector<int> dirs = {0, 1, 2, 3};
            std::shuffle(dirs.begin(), dirs.end(), rng);
            bool moved = false;
            for (int d : dirs) {
                int nr = cr + dr[d];
                int nc = cc + dc[d];
                if (nr > 0 && nr < rows - 1 &&
                    nc > 0 && nc < cols - 1 &&
                    lines[nr][nc] == '#') {
                    lines[cr + dr[d]/2][cc + dc[d]/2] = ' ';
                    lines[nr][nc] = ' ';
                    stk.push({nr, nc});
                    moved = true;
                    break;
                }
            }
            if (!moved) stk.pop();
        }
    };

    carve(1, 1);
    lines[1][1]           = 'S';
    lines[rows-2][cols-2] = 'E';
    return Maze(lines);
}

// ==============================================================
//  DISPLAY WITH LIGHT BLUE EXPLORED NODES
// ==============================================================
void Maze::display(
    const std::vector<Point>* exploredOverlay,
    const std::vector<Point>* pathOverlay
) const {
    // Build lookup grids for O(1) checking
    std::vector<std::vector<bool>> onPath(
        rows_, std::vector<bool>(cols_, false));
    std::vector<std::vector<bool>> onExplored(
        rows_, std::vector<bool>(cols_, false));

    if (pathOverlay)
        for (const auto& p : *pathOverlay)
            onPath[p.row][p.col] = true;

    if (exploredOverlay)
        for (const auto& p : *exploredOverlay)
            onExplored[p.row][p.col] = true;

    // Top border
    std::cout << Color::YELLOW << "+";
    for (int c = 0; c < cols_; c++) std::cout << "--";
    std::cout << "+\n" << Color::RESET;

    for (int r = 0; r < rows_; r++) {
        std::cout << Color::YELLOW << "|" << Color::RESET;

        for (int c = 0; c < cols_; c++) {
            char ch         = grid_[r][c];
            bool isPath     = onPath[r][c];
            bool isExplored = onExplored[r][c];

            // Priority: Wall > Start > End > Path > Explored > Open
            if (ch == '#') {
                // ---- WALL: white background ----
                std::cout << Color::BG_WHITE << Color::BOLD
                          << "  "
                          << Color::RESET;

            } else if (ch == 'S') {
                // ---- START: green background ----
                std::cout << Color::BG_GREEN << Color::BOLD
                          << " S"
                          << Color::RESET;

            } else if (ch == 'E') {
                // ---- END: red background ----
                std::cout << Color::BG_RED << Color::BOLD
                          << " E"
                          << Color::RESET;

            } else if (isPath) {
                // ---- SOLUTION PATH: yellow background ----
                std::cout << Color::BG_YELLOW << Color::BOLD
                          << ">>"
                          << Color::RESET;

            } else if (isExplored) {
                // ---- EXPLORED NODE: LIGHT BLUE background ----
                std::cout << Color::BG_LIGHT_BLUE << Color::BOLD
                          << "  "
                          << Color::RESET;

            } else {
                // ---- OPEN SPACE ----
                std::cout << "  ";
            }
        }

        std::cout << Color::YELLOW << "|\n" << Color::RESET;
    }

    // Bottom border
    std::cout << Color::YELLOW << "+";
    for (int c = 0; c < cols_; c++) std::cout << "--";
    std::cout << "+\n" << Color::RESET;
}

// ==============================================================
//  DISPLAY SOLUTION — pass explored + path to display()
// ==============================================================
void Maze::displaySolution(const SolveResult& result) const {
    std::cout << "\n";
    Utils::printSeparator(result.algorithmName + " Solution");

    if (!result.found) {
        std::cout << Color::RED << "  [!] No path found!\n" << Color::RESET;
        // Show explored nodes even when no path found
        if (!result.exploredNodes.empty())
            display(&result.exploredNodes, nullptr);
        else
            display();
        return;
    }

    std::cout << "\n";
    // Pass BOTH explored nodes AND solution path
    display(&result.exploredNodes, &result.path);

    std::cout << "\n";
    std::cout << Color::GREEN << Color::BOLD
              << "  [OK] Path found!\n" << Color::RESET;
    std::cout << Color::CYAN
              << "  Path length    : " << result.path.size()    << " steps\n"
              << "  Nodes explored : " << result.nodesExplored  << "\n"
              << "  Time taken     : " << result.timeMs         << " ms\n"
              << Color::RESET;
}

// ==============================================================
//  COMPARISON TABLE
// ==============================================================
void Maze::displayComparison(
    const SolveResult& bfs,
    const SolveResult& dfs
) const {
    std::cout << "\n";
    Utils::printSeparator("Algorithm Comparison");
    std::cout << "\n";

    auto pad = [](const std::string& s, int w) -> std::string {
        if ((int)s.size() >= w) return s;
        return s + std::string(w - s.size(), ' ');
    };

    // Header
    std::cout << Color::BOLD
              << pad("Metric", 24)
              << pad("BFS", 20)
              << "DFS\n"
              << Color::RESET;
    std::cout << std::string(60, '-') << "\n";

    auto row = [&](const std::string& label,
                   const std::string& v1,
                   const std::string& v2) {
        std::cout << pad(label, 24)
                  << Color::GREEN  << pad(v1, 20) << Color::RESET
                  << Color::YELLOW << v2           << Color::RESET
                  << "\n";
    };

    row("Path Found",
        bfs.found ? "Yes" : "No",
        dfs.found ? "Yes" : "No");

    row("Path Length",
        bfs.found ? std::to_string(bfs.path.size()) + " steps" : "N/A",
        dfs.found ? std::to_string(dfs.path.size()) + " steps" : "N/A");

    row("Nodes Explored",
        std::to_string(bfs.nodesExplored),
        std::to_string(dfs.nodesExplored));

    row("Time (ms)",
        std::to_string(bfs.timeMs),
        std::to_string(dfs.timeMs));

    std::cout << "\n";

    // Verdict
    if (bfs.found && dfs.found) {
        if (bfs.path.size() <= dfs.path.size())
            std::cout << Color::GREEN << Color::BOLD
                      << "  [WIN] BFS found shorter path ("
                      << bfs.path.size() << " vs "
                      << dfs.path.size() << " steps)\n"
                      << Color::RESET;
        else
            std::cout << Color::YELLOW << Color::BOLD
                      << "  [WIN] DFS found shorter path ("
                      << dfs.path.size() << " vs "
                      << bfs.path.size() << " steps)\n"
                      << Color::RESET;

        if (bfs.nodesExplored <= dfs.nodesExplored)
            std::cout << Color::GREEN
                      << "  [INFO] BFS explored fewer nodes ("
                      << bfs.nodesExplored << " vs "
                      << dfs.nodesExplored << ")\n"
                      << Color::RESET;
        else
            std::cout << Color::YELLOW
                      << "  [INFO] DFS explored fewer nodes ("
                      << dfs.nodesExplored << " vs "
                      << bfs.nodesExplored << ")\n"
                      << Color::RESET;
    }

    // Legend with actual color samples
    std::cout << "\n";
    Utils::printSeparator("Legend");
    std::cout << Color::BG_GREEN      << Color::BOLD << " S  " << Color::RESET
              << " = Start\n";
    std::cout << Color::BG_RED        << Color::BOLD << " E  " << Color::RESET
              << " = End\n";
    std::cout << Color::BG_YELLOW     << Color::BOLD << " >> " << Color::RESET
              << " = Solution Path\n";
    std::cout << Color::BG_LIGHT_BLUE << Color::BOLD << "    " << Color::RESET
              << " = Explored Node (light blue)\n";
    std::cout << Color::BG_WHITE      << Color::BOLD << "    " << Color::RESET
              << " = Wall\n";
    std::cout << "       = Open space\n";
}