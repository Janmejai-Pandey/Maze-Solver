#pragma once
#include "Solver.h"
#include <vector>
#include <string>

class Maze {
public:
    explicit Maze(const std::vector<std::string>& lines);
    static Maze generateRandom(int rows, int cols, unsigned int seed = 42);

    // Display with explored nodes (light blue) and path overlay (yellow)
    void display(
        const std::vector<Point>* exploredOverlay = nullptr,
        const std::vector<Point>* pathOverlay     = nullptr
    ) const;

    void displaySolution(const SolveResult& result) const;
    void displayComparison(const SolveResult& bfs, const SolveResult& dfs) const;

    const std::vector<std::vector<char>>& getGrid()  const { return grid_; }
    Point getStart() const { return start_; }
    Point getEnd()   const { return end_;   }
    int   getRows()  const { return rows_;  }
    int   getCols()  const { return cols_;  }

private:
    std::vector<std::vector<char>> grid_;
    Point start_, end_;
    int   rows_, cols_;
    void parseStartEnd();
};