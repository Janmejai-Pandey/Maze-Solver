#include "MazeRenderer.h"
#include <algorithm>

MazeRenderer::MazeRenderer(float x, float y, float maxW, float maxH)
    : x_(x), y_(y), maxW_(maxW), maxH_(maxH) {}

void MazeRenderer::setMaze(const Maze& maze) {
    maze_ = &maze;
    clearResults();

    int rows = maze.getRows(), cols = maze.getCols();
    if (rows == 0 || cols == 0) return;

    float cellW = maxW_ / cols;
    float cellH = maxH_ / rows;
    cellSize_ = std::min(cellW, cellH);
    cellSize_ = std::max(cellSize_, 2.f);

    float totalW = cellSize_ * cols;
    float totalH = cellSize_ * rows;
    offsetX_ = x_ + (maxW_ - totalW) / 2.f;
    offsetY_ = y_ + (maxH_ - totalH) / 2.f;
}

void MazeRenderer::setSolveResult(const SolveResult& result, bool isBFS) {
    if (isBFS) { bfsResult_ = result; hasBFS_ = true; }
    else       { dfsResult_ = result; hasDFS_ = true; }
}

void MazeRenderer::clearResults() {
    hasBFS_ = hasDFS_ = false;
    animating_ = false;
    animStep_ = 0;
}

Point MazeRenderer::screenToGrid(float mx, float my) const {
    int c = (int)((mx - offsetX_) / cellSize_);
    int r = (int)((my - offsetY_) / cellSize_);
    return {r, c};
}

bool MazeRenderer::containsPoint(float mx, float my) const {
    if (!maze_) return false;
    return mx >= offsetX_ && my >= offsetY_
        && mx < offsetX_ + cellSize_ * maze_->getCols()
        && my < offsetY_ + cellSize_ * maze_->getRows();
}

void MazeRenderer::update(float dt) {
    if (!animating_) return;
    animTimer_ += dt;
    float stepTime = 1.f / animSpeed_;
    while (animTimer_ >= stepTime) {
        animTimer_ -= stepTime;
        animStep_++;

        int maxSteps = 0;
        if (showBFS_ && hasBFS_)
            maxSteps = std::max(maxSteps,
                (int)(bfsResult_.exploredNodes.size()
                    + bfsResult_.path.size()));
        if (showDFS_ && hasDFS_)
            maxSteps = std::max(maxSteps,
                (int)(dfsResult_.exploredNodes.size()
                    + dfsResult_.path.size()));

        if (animStep_ >= maxSteps) {
            animating_ = false;
            animStep_  = 999999;
            break;
        }
    }
}

void MazeRenderer::draw(sf::RenderWindow& window) {
    if (!maze_ || !maze_->isValid()) return;

    auto& grid = maze_->getGrid();
    int rows   = maze_->getRows();
    int cols   = maze_->getCols();

    auto buildLookup = [&](const std::vector<Point>& pts, int limit = -1) {
        std::vector<std::vector<bool>> lk(
            rows, std::vector<bool>(cols, false));
        int cnt = (limit < 0)
                  ? (int)pts.size()
                  : std::min(limit, (int)pts.size());
        for (int i = 0; i < cnt; i++)
            lk[pts[i].row][pts[i].col] = true;
        return lk;
    };

    int bfsExpLimit = -1, bfsPathLimit = -1;
    int dfsExpLimit = -1, dfsPathLimit = -1;

    if (animating_) {
        if (showBFS_ && hasBFS_) {
            int es = (int)bfsResult_.exploredNodes.size();
            bfsExpLimit  = std::min(animStep_, es);
            bfsPathLimit = std::max(0, animStep_ - es);
        }
        if (showDFS_ && hasDFS_) {
            int es = (int)dfsResult_.exploredNodes.size();
            dfsExpLimit  = std::min(animStep_, es);
            dfsPathLimit = std::max(0, animStep_ - es);
        }
    }

    std::vector<std::vector<bool>> bfsExp, bfsPath, dfsExp, dfsPath;
    if (showBFS_ && hasBFS_ && showExplored_)
        bfsExp  = buildLookup(bfsResult_.exploredNodes, bfsExpLimit);
    if (showBFS_ && hasBFS_)
        bfsPath = buildLookup(bfsResult_.path, bfsPathLimit);
    if (showDFS_ && hasDFS_ && showExplored_)
        dfsExp  = buildLookup(dfsResult_.exploredNodes, dfsExpLimit);
    if (showDFS_ && hasDFS_)
        dfsPath = buildLookup(dfsResult_.path, dfsPathLimit);

    sf::RectangleShape cell;
    cell.setSize({cellSize_ - 1.f, cellSize_ - 1.f});

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            float px = offsetX_ + c * cellSize_;
            float py = offsetY_ + r * cellSize_;
            cell.setPosition({px, py});        // SFML 3: Vector2f

            char ch = grid[r][c];
            sf::Color color;

            if (ch == '#') {
                color = MazeColors::WALL;
            } else if (ch == 'S') {
                color = MazeColors::START;
            } else if (ch == 'E') {
                color = MazeColors::END_PT;
            } else {
                color = MazeColors::PATH_OPEN;

                bool bfsE = !bfsExp.empty()  && bfsExp[r][c];
                bool dfsE = !dfsExp.empty()  && dfsExp[r][c];
                bool bfsP = !bfsPath.empty() && bfsPath[r][c];
                bool dfsP = !dfsPath.empty() && dfsPath[r][c];

                if      (bfsP && showBFS_) color = MazeColors::BFS_PATH;
                else if (dfsP && showDFS_) color = MazeColors::DFS_PATH;
                else if (bfsE && showBFS_) color = MazeColors::BFS_EXPLORED;
                else if (dfsE && showDFS_) color = MazeColors::DFS_EXPLORED;

                if (bfsP && dfsP && showBFS_ && showDFS_) {
                    color = sf::Color(
                        (MazeColors::BFS_PATH.r + MazeColors::DFS_PATH.r)/2,
                        (MazeColors::BFS_PATH.g + MazeColors::DFS_PATH.g)/2,
                        (MazeColors::BFS_PATH.b + MazeColors::DFS_PATH.b)/2
                    );
                }
            }

            cell.setFillColor(color);
            window.draw(cell);
        }
    }

    // Grid lines (only when cells are large enough)
    if (cellSize_ > 6.f) {
        sf::RectangleShape gridLine;
        gridLine.setFillColor(MazeColors::GRID_LINE);

        for (int c = 0; c <= cols; c++) {
            float px = offsetX_ + c * cellSize_;
            gridLine.setPosition({px - 0.5f, offsetY_});   // SFML 3
            gridLine.setSize({1.f, rows * cellSize_});
            window.draw(gridLine);
        }
        for (int r = 0; r <= rows; r++) {
            float py = offsetY_ + r * cellSize_;
            gridLine.setPosition({offsetX_, py - 0.5f});   // SFML 3
            gridLine.setSize({cols * cellSize_, 1.f});
            window.draw(gridLine);
        }
    }
}