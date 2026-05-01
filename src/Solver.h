#pragma once
#include <vector>
#include <string>
#include <chrono>

struct Point {
    int row, col;
    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

struct SolveResult {
    bool                found;
    std::vector<Point>  path;            // final solution path
    std::vector<Point>  exploredNodes;   // ALL nodes visited during search
    int                 nodesExplored;
    long long           timeMs;
    std::string         algorithmName;
};

class Solver {
public:
    virtual ~Solver() = default;
    virtual SolveResult solve(
        const std::vector<std::vector<char>>& grid,
        Point start,
        Point end
    ) = 0;
    virtual std::string getName() const = 0;
};