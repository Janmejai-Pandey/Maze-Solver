#pragma once
#include "Maze.h"
#include "Solver.h"
#include "Colors.h"
#include <SFML/Graphics.hpp>

class MazeRenderer {
public:
    MazeRenderer(float x, float y, float maxW, float maxH);

    void setMaze(const Maze& maze);
    void setSolveResult(const SolveResult& result, bool isBFS);
    void clearResults();

    void setShowBFS(bool s)     { showBFS_ = s; }
    void setShowDFS(bool s)     { showDFS_ = s; }
    void setShowExplored(bool s){ showExplored_ = s; }
    void setAnimating(bool a)   { animating_ = a; animStep_ = 0; }

    void update(float dt);
    void draw(sf::RenderWindow& window);

    // Convert mouse position to grid cell
    Point screenToGrid(float mx, float my) const;
    bool  containsPoint(float mx, float my) const;

private:
    float x_, y_, maxW_, maxH_;
    float cellSize_ = 0;
    float offsetX_ = 0, offsetY_ = 0;

    const Maze* maze_ = nullptr;
    SolveResult bfsResult_, dfsResult_;
    bool hasBFS_ = false, hasDFS_ = false;
    bool showBFS_ = true, showDFS_ = false;
    bool showExplored_ = true;
    bool animating_ = false;
    int  animStep_ = 0;
    float animTimer_ = 0;
    float animSpeed_ = 50.f; // steps per second
};