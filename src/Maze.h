#pragma once
#include "Solver.h"
#include <vector>
#include <string>

class Maze {
public:
    Maze();
    explicit Maze(const std::vector<std::string>& lines);

    static Maze generateRandom(int rows, int cols, unsigned int seed = 42);

    bool isValid() const { return valid_; }

    const std::vector<std::vector<char>>& getGrid() const { return grid_; }
    Point getStart() const { return start_; }
    Point getEnd()   const { return end_; }
    int   getRows()  const { return rows_; }
    int   getCols()  const { return cols_; }

    // Allow setting start/end via mouse clicks
    void setStart(int r, int c);
    void setEnd(int r, int c);
    void setGrid(const std::vector<std::vector<char>>& grid);

private:
    std::vector<std::vector<char>> grid_;
    Point start_{0, 0}, end_{0, 0};
    int rows_ = 0, cols_ = 0;
    bool valid_ = false;

    void parseStartEnd();
};