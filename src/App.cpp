#include "App.h"
#include "Button.h"
#include "Colors.h"
#include <iostream>
#include <random>

// ── Preset Mazes ──────────────────────────────────────────────
static const std::vector<std::vector<std::string>> PRESETS = {
    {   // 0: Simple
        "###########",
        "S         #",
        "# ####### #",
        "# #       #",
        "# # ##### #",
        "# # #   # #",
        "# # # # # #",
        "# #   #   #",
        "# ####### #",
        "#         E",
        "###########"
    },
    {   // 1: Medium
        "###################",
        "S   #       #     #",
        "### # ##### # ### #",
        "#   #     # # #   #",
        "# ##### ### # # ###",
        "#   #   #   #   # #",
        "### # ### ####### #",
        "#   # #       #   #",
        "# ### # ##### # ###",
        "#   # # #   # #   #",
        "### ### # # # ### #",
        "#       # #     # #",
        "# ####### ####### #",
        "#   #           # #",
        "### # ########### #",
        "#   #             E",
        "###################"
    },
    {   // 2: Large
        "#########################",
        "S #     #       #       #",
        "# # ### # ##### # ##### #",
        "#   #   # #   #   #   # #",
        "##### ### # # ##### # # #",
        "#       # # #       # # #",
        "# ####### # ######### # #",
        "# #       #     #     # #",
        "# # ########### # ##### #",
        "#   #           #       #",
        "# ### ################## #",
        "#   # #       #       # #",
        "### # # ##### # ##### # #",
        "#   #   #   # # #   #   #",
        "# ####### # # # # # #####",
        "#         # #   # #     #",
        "# ######### ##### ##### #",
        "#                       E",
        "#########################"
    },
    {   // 3: Classic (square)
        "#############",
        "S     #     #",
        "# ### # ### #",
        "# # #   # # #",
        "# # ##### # #",
        "#   #   #   #",
        "### # # # ###",
        "#     #     #",
        "# ##### ### #",
        "# #   #   # #",
        "# # ### # # #",
        "#       #   E",
        "#############"
    }
};

// ── Constructor ───────────────────────────────────────────────
App::App()
    : window_(sf::VideoMode({(unsigned int)WIN_W, (unsigned int)WIN_H}),
              "Maze Solver - BFS vs DFS",
              sf::Style::Close | sf::Style::Titlebar),
      renderer_(PANEL_W + 10, 10, WIN_W - PANEL_W - 20, WIN_H - 20)
{
    window_.setFramerateLimit(60);

    // Load font
    bool fontLoaded = font_.openFromFile("assets/fonts/arial.ttf");

    if (!fontLoaded) {
#ifdef _WIN32
        fontLoaded = font_.openFromFile("C:/Windows/Fonts/arial.ttf");
        if (!fontLoaded)
            fontLoaded = font_.openFromFile("C:/Windows/Fonts/segoeui.ttf");
#elif __APPLE__
        fontLoaded = font_.openFromFile("/Library/Fonts/Arial.ttf");
        if (!fontLoaded)
            fontLoaded = font_.openFromFile(
                "/System/Library/Fonts/Helvetica.ttc");
#else
        fontLoaded = font_.openFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        if (!fontLoaded)
            fontLoaded = font_.openFromFile(
                "/usr/share/fonts/liberation/"
                "LiberationSans-Regular.ttf");
#endif
    }

    if (!fontLoaded)
        std::cerr << "WARNING: No font loaded! "
                     "Place arial.ttf in assets/fonts/\n";

    // Build panel now that font is ready
    panel_ = UIPanel(0, 0, PANEL_W, WIN_H, font_);

    setupButtons();
    loadPresetMaze(0);
}

// ── Setup Buttons ─────────────────────────────────────────────
void App::setupButtons()
{
    float bx  = 10.f;
    float bw  = PANEL_W - 20.f;
    float bh  = 28.f;
    float by  = 100.f;
    float gap = 33.f;

    auto addBtn = [&](const std::string& label,
                      std::function<void()> cb)
    {
        MazeButton btn(bx, by, bw, bh, label, font_);
        btn.setCallback(cb);
        panel_.addButton(btn);
        by += gap;
    };

    addBtn("Simple Maze (11x11)",  [this]{ loadPresetMaze(0); });
    addBtn("Medium Maze (19x17)",  [this]{ loadPresetMaze(1); });
    addBtn("Large Maze  (25x19)",  [this]{ loadPresetMaze(2); });
    addBtn("Classic Maze (13x13)", [this]{ loadPresetMaze(3); });
    addBtn("Random Maze",          [this]{ generateRandomMaze(); });

    by += 10.f;

    addBtn("Solve BFS",            [this]{ solveBFS(); });
    addBtn("Solve DFS",            [this]{ solveDFS(); });
    addBtn("Solve Both (BFS+DFS)", [this]{ solveAll(); });
    addBtn("Clear Solution",       [this]{ clearSolution(); });

    by += 10.f;

    addBtn("Toggle BFS Path",      [this]{ toggleBFS(); });
    addBtn("Toggle DFS Path",      [this]{ toggleDFS(); });
    addBtn("Toggle Explored Nodes",[this]{ toggleExplored(); });
    addBtn("Animate Solution",     [this]{ toggleAnimate(); });

    by += 10.f;

    addBtn("Set Start (click)",    [this]{ placeStart(); });
    addBtn("Set End   (click)",    [this]{ placeEnd(); });
}

// ── Maze Loading ──────────────────────────────────────────────
void App::loadPresetMaze(int id)
{
    if (id < 0 || id >= (int)PRESETS.size()) return;
    maze_ = Maze(PRESETS[id]);
    updateMaze();
    panel_.setStatus("Preset " + std::to_string(id + 1) + " loaded");
}

void App::generateRandomMaze()
{
    std::random_device rd;
    maze_ = Maze::generateRandom(21, 31, rd());
    updateMaze();
    panel_.setStatus("Random maze generated");
}

void App::updateMaze()
{
    renderer_.setMaze(maze_);
    panel_.setMazeInfo(maze_.getRows(), maze_.getCols());
    panel_.clearResults();
    placeMode_ = PlaceMode::NONE;
}

// ── Solvers ───────────────────────────────────────────────────
void App::solveBFS()
{
    if (!maze_.isValid()) { panel_.setStatus("No valid maze!"); return; }
    auto r = bfsSolver_.solve(
        maze_.getGrid(), maze_.getStart(), maze_.getEnd());
    renderer_.setSolveResult(r, true);
    panel_.setBFSResult(r);
    showBFS_ = true;
    renderer_.setShowBFS(true);
    panel_.setStatus("BFS: " + (r.found
        ? std::to_string(r.path.size()) + " steps"
        : "no path found"));
}

void App::solveDFS()
{
    if (!maze_.isValid()) { panel_.setStatus("No valid maze!"); return; }
    auto r = dfsSolver_.solve(
        maze_.getGrid(), maze_.getStart(), maze_.getEnd());
    renderer_.setSolveResult(r, false);
    panel_.setDFSResult(r);
    showDFS_ = true;
    renderer_.setShowDFS(true);
    panel_.setStatus("DFS: " + (r.found
        ? std::to_string(r.path.size()) + " steps"
        : "no path found"));
}

void App::solveAll()
{
    solveBFS();
    solveDFS();
    showBFS_ = showDFS_ = true;
    renderer_.setShowBFS(true);
    renderer_.setShowDFS(true);
    panel_.setStatus("Both BFS and DFS solved!");
}

void App::clearSolution()
{
    renderer_.clearResults();
    panel_.clearResults();
    panel_.setStatus("Solution cleared");
}

// ── Toggles ───────────────────────────────────────────────────
void App::toggleBFS()
{
    showBFS_ = !showBFS_;
    renderer_.setShowBFS(showBFS_);
    panel_.setStatus(std::string("BFS view: ")
                     + (showBFS_ ? "ON" : "OFF"));
}

void App::toggleDFS()
{
    showDFS_ = !showDFS_;
    renderer_.setShowDFS(showDFS_);
    panel_.setStatus(std::string("DFS view: ")
                     + (showDFS_ ? "ON" : "OFF"));
}

void App::toggleExplored()
{
    showExplored_ = !showExplored_;
    renderer_.setShowExplored(showExplored_);
    panel_.setStatus(std::string("Explored nodes: ")
                     + (showExplored_ ? "ON" : "OFF"));
}

void App::toggleAnimate()
{
    renderer_.setAnimating(true);
    panel_.setStatus("Animating solution...");
}

void App::placeStart()
{
    placeMode_ = PlaceMode::START;
    panel_.setStatus("Click the maze to set START");
}

void App::placeEnd()
{
    placeMode_ = PlaceMode::END;
    panel_.setStatus("Click the maze to set END");
}

// ── Mouse click on maze grid ──────────────────────────────────
void App::handleMazeClick(float mx, float my)
{
    if (!renderer_.containsPoint(mx, my)) return;

    Point p = renderer_.screenToGrid(mx, my);
    if (p.row < 0 || p.row >= maze_.getRows() ||
        p.col < 0 || p.col >= maze_.getCols())
        return;

    if (maze_.getGrid()[p.row][p.col] == '#') {
        panel_.setStatus("Cannot place on a wall!");
        return;
    }

    if (placeMode_ == PlaceMode::START) {
        maze_.setStart(p.row, p.col);
        updateMaze();
        placeMode_ = PlaceMode::NONE;
        panel_.setStatus("Start placed at ("
            + std::to_string(p.row) + ","
            + std::to_string(p.col) + ")");
    }
    else if (placeMode_ == PlaceMode::END) {
        maze_.setEnd(p.row, p.col);
        updateMaze();
        placeMode_ = PlaceMode::NONE;
        panel_.setStatus("End placed at ("
            + std::to_string(p.row) + ","
            + std::to_string(p.col) + ")");
    }
}

// ── Event Loop ────────────────────────────────────────────────
void App::handleEvents()
{
    while (auto event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window_.close();

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
        {
            using Key = sf::Keyboard::Key;
            switch (kp->code)
            {
                case Key::Num1:   loadPresetMaze(0);    break;
                case Key::Num2:   loadPresetMaze(1);    break;
                case Key::Num3:   loadPresetMaze(2);    break;
                case Key::Num4:   loadPresetMaze(3);    break;
                case Key::R:      generateRandomMaze(); break;
                case Key::B:      solveBFS();           break;
                case Key::D:      solveDFS();           break;
                case Key::A:      solveAll();           break;
                case Key::C:      clearSolution();      break;
                case Key::E:      toggleExplored();     break;
                case Key::Space:  toggleAnimate();      break;
                case Key::Escape: window_.close();      break;
                default: break;
            }
        }

        if (const auto* mb =
                event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mb->button == sf::Mouse::Button::Left &&
                placeMode_ != PlaceMode::NONE)
            {
                handleMazeClick((float)mb->position.x,
                                (float)mb->position.y);
            }
        }

        panel_.handleEvent(*event, window_);
    }
}

// ── Update & Render ───────────────────────────────────────────
void App::update(float dt)
{
    renderer_.update(dt);
}

void App::render()
{
    window_.clear(MazeColors::BACKGROUND);
    renderer_.draw(window_);
    panel_.draw(window_);
    window_.display();
}

// ── Main Loop ─────────────────────────────────────────────────
void App::run()
{
    sf::Clock clock;
    while (window_.isOpen())
    {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}