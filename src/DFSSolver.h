#pragma once
#include "Solver.h"

class DFSSolver : public Solver {
public:
    SolveResult solve(
        const std::vector<std::vector<char>>& grid,
        Point start, Point end
    ) override;
    std::string getName() const override { return "DFS"; }
};