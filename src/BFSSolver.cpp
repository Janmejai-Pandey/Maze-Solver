#include "BFSSolver.h"
#include <queue>
#include <unordered_map>
#include <chrono>
#include <algorithm>

SolveResult BFSSolver::solve(
    const std::vector<std::vector<char>>& grid,
    Point start, Point end)
{
    auto t0 = std::chrono::high_resolution_clock::now();

    int rows = (int)grid.size();
    int cols = (int)grid[0].size();
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    std::queue<Point> q;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::unordered_map<int, Point> parent;
    std::unordered_map<int, bool>  hasParent;

    auto enc = [&](Point p) { return p.row * cols + p.col; };

    q.push(start);
    visited[start.row][start.col] = true;
    hasParent[enc(start)] = false;

    int explored = 0;
    bool found = false;
    std::vector<Point> exploredList;

    while (!q.empty()) {
        Point cur = q.front(); q.pop();
        explored++;
        exploredList.push_back(cur);

        if (cur == end) { found = true; break; }

        for (int d = 0; d < 4; d++) {
            int nr = cur.row + dr[d], nc = cur.col + dc[d];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (visited[nr][nc] || grid[nr][nc] == '#') continue;
            visited[nr][nc] = true;
            Point nxt = {nr, nc};
            parent[enc(nxt)] = cur;
            hasParent[enc(nxt)] = true;
            q.push(nxt);
        }
    }

    std::vector<Point> path;
    if (found) {
        Point cur = end;
        while (hasParent[enc(cur)]) {
            path.push_back(cur);
            cur = parent[enc(cur)];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    return {found, path, exploredList, explored, ms, getName()};
}