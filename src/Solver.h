#pragma once
#include <vector>
#include <string>

struct Point {
    int row, col;
    bool operator==(const Point& o) const { return row == o.row && col == o.col; }
    bool operator!=(const Point& o) const { return !(*this == o); }
};

struct SolveResult {
    bool                found;
    std::vector<Point>  path;
    std::vector<Point>  exploredNodes;
    int                 nodesExplored;
    double              timeMs;
    std::string         algorithmName;
};

class Solver {
public:
    virtual ~Solver() = default;
    virtual SolveResult solve(
        const std::vector<std::vector<char>>& grid,
        Point start, Point end
    ) = 0;
    virtual std::string getName() const = 0;
};