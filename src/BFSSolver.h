#pragma once
#include "Solver.h"

class BFSSolver : public Solver {
public:
    SolveResult solve(
        const std::vector<std::vector<char>>& grid,
        Point start,
        Point end
    ) override;

    std::string getName() const override { return "BFS (Breadth-First Search)"; }
};