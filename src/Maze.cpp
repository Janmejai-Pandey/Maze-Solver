#include "Maze.h"
#include <stdexcept>
#include <algorithm>
#include <random>
#include <stack>

Maze::Maze() : valid_(false) {}

Maze::Maze(const std::vector<std::string>& lines) {
    if (lines.empty()) { valid_ = false; return; }
    rows_ = (int)lines.size();
    cols_ = 0;
    for (auto& l : lines) cols_ = std::max(cols_, (int)l.size());

    grid_.resize(rows_, std::vector<char>(cols_, '#'));
    for (int r = 0; r < rows_; r++)
        for (int c = 0; c < (int)lines[r].size(); c++)
            grid_[r][c] = lines[r][c];

    parseStartEnd();
}

void Maze::parseStartEnd() {
    bool fS = false, fE = false;
    for (int r = 0; r < rows_; r++)
        for (int c = 0; c < cols_; c++) {
            if (grid_[r][c] == 'S') { start_ = {r, c}; fS = true; }
            if (grid_[r][c] == 'E') { end_   = {r, c}; fE = true; }
        }
    valid_ = fS && fE;
}

void Maze::setStart(int r, int c) {
    if (r >= 0 && r < rows_ && c >= 0 && c < cols_) {
        // Clear old start
        for (int i = 0; i < rows_; i++)
            for (int j = 0; j < cols_; j++)
                if (grid_[i][j] == 'S') grid_[i][j] = ' ';
        grid_[r][c] = 'S';
        start_ = {r, c};
        parseStartEnd();
    }
}

void Maze::setEnd(int r, int c) {
    if (r >= 0 && r < rows_ && c >= 0 && c < cols_) {
        for (int i = 0; i < rows_; i++)
            for (int j = 0; j < cols_; j++)
                if (grid_[i][j] == 'E') grid_[i][j] = ' ';
        grid_[r][c] = 'E';
        end_ = {r, c};
        parseStartEnd();
    }
}

void Maze::setGrid(const std::vector<std::vector<char>>& grid) {
    grid_ = grid;
    rows_ = (int)grid.size();
    cols_ = rows_ > 0 ? (int)grid[0].size() : 0;
    parseStartEnd();
}

Maze Maze::generateRandom(int rows, int cols, unsigned int seed) {
    if (rows % 2 == 0) rows++;
    if (cols % 2 == 0) cols++;
    std::vector<std::string> lines(rows, std::string(cols, '#'));
    std::mt19937 rng(seed);

    struct Frame { int r, c; };
    std::stack<Frame> stk;
    stk.push({1, 1});
    lines[1][1] = ' ';
    int dr[] = {-2, 2, 0, 0};
    int dc[] = { 0, 0, -2, 2};

    while (!stk.empty()) {
        auto [cr, cc] = stk.top();
        std::vector<int> dirs = {0,1,2,3};
        std::shuffle(dirs.begin(), dirs.end(), rng);
        bool moved = false;
        for (int d : dirs) {
            int nr = cr + dr[d], nc = cc + dc[d];
            if (nr > 0 && nr < rows-1 && nc > 0 && nc < cols-1
                && lines[nr][nc] == '#') {
                lines[cr + dr[d]/2][cc + dc[d]/2] = ' ';
                lines[nr][nc] = ' ';
                stk.push({nr, nc});
                moved = true; break;
            }
        }
        if (!moved) stk.pop();
    }

    lines[1][1] = 'S';
    lines[rows-2][cols-2] = 'E';
    return Maze(lines);
}