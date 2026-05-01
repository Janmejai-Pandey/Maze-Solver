#include "BFSSolver.h"
#include <queue>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>

SolveResult BFSSolver::solve(
    const std::vector<std::vector<char>>& grid,
    Point start,
    Point end
) {
    auto startTime = std::chrono::high_resolution_clock::now();

    const int rows = (int)grid.size();
    const int cols = (int)grid[0].size();
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};

    std::queue<Point> bfsQueue;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::unordered_map<int, Point> parent;
    std::unordered_map<int, bool>  hasParent;

    auto encode = [&](Point p) { return p.row * cols + p.col; };

    bfsQueue.push(start);
    visited[start.row][start.col] = true;
    hasParent[encode(start)] = false;

    int nodesExplored = 0;
    bool found = false;
    std::vector<Point> explored;  // track every visited node

    while (!bfsQueue.empty()) {
        Point cur = bfsQueue.front();
        bfsQueue.pop();
        nodesExplored++;
        explored.push_back(cur);  // record this node

        if (cur == end) { found = true; break; }

        for (int d = 0; d < 4; d++) {
            int nr = cur.row + dr[d];
            int nc = cur.col + dc[d];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (visited[nr][nc] || grid[nr][nc] == '#') continue;
            visited[nr][nc] = true;
            Point next = {nr, nc};
            parent[encode(next)] = cur;
            hasParent[encode(next)] = true;
            bfsQueue.push(next);
        }
    }

    // Reconstruct path
    std::vector<Point> path;
    if (found) {
        Point cur = end;
        while (hasParent[encode(cur)]) {
            path.push_back(cur);
            cur = parent[encode(cur)];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();

    return { found, path, explored, nodesExplored, ms, getName() };
}