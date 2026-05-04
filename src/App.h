#pragma once
#include "Maze.h"
#include "MazeRenderer.h"
#include "UIPanel.h"
#include "BFSSolver.h"
#include "DFSSolver.h"
#include <SFML/Graphics.hpp>
#include <string>

enum class PlaceMode { NONE, START, END };

class App {
public:
    App();
    void run();

private:
    sf::RenderWindow window_;
    sf::Font         font_;

    Maze         maze_;
    MazeRenderer renderer_;
    UIPanel      panel_;
    BFSSolver    bfsSolver_;
    DFSSolver    dfsSolver_;

    PlaceMode   placeMode_    = PlaceMode::NONE;
    bool        showBFS_      = true;
    bool        showDFS_      = false;
    bool        showExplored_ = true;

    static const int WIN_W   = 1200;
    static const int WIN_H   = 800;
    static const int PANEL_W = 280;

    void setupButtons();
    void handleEvents();
    void update(float dt);
    void render();

    void loadPresetMaze(int id);
    void generateRandomMaze();
    void solveBFS();
    void solveDFS();
    void solveAll();
    void clearSolution();
    void toggleBFS();
    void toggleDFS();
    void toggleExplored();
    void toggleAnimate();
    void placeStart();
    void placeEnd();

    void updateMaze();
    void handleMazeClick(float mx, float my);
};