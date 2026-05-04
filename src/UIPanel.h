#pragma once
#include "Button.h"
#include "Solver.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class UIPanel {
public:
    // Default constructor — needed so App can declare it without args
    UIPanel() : x_(0), y_(0), w_(0), h_(0), font_(nullptr) {}

    UIPanel(float x, float y, float w, float h, const sf::Font& font)
        : x_(x), y_(y), w_(w), h_(h), font_(&font) {}

    // Allow copy/move assignment (pointer is copyable)
    UIPanel(const UIPanel&)            = default;
    UIPanel& operator=(const UIPanel&) = default;
    UIPanel(UIPanel&&)                 = default;
    UIPanel& operator=(UIPanel&&)      = default;

    void setBFSResult(const SolveResult& r) { bfs_ = r; hasBFS_ = true; }
    void setDFSResult(const SolveResult& r) { dfs_ = r; hasDFS_ = true; }
    void clearResults()                     { hasBFS_ = hasDFS_ = false; }
    void setStatus(const std::string& s)    { status_ = s; }
    void setMazeInfo(int rows, int cols)    { mazeRows_ = rows; mazeCols_ = cols; }

    std::vector<MazeButton>& getButtons() { return buttons_; }
    void addButton(const MazeButton& btn) { buttons_.push_back(btn); }

    void handleEvent(const sf::Event& e, const sf::RenderWindow& w);
    void draw(sf::RenderWindow& window);

private:
    float x_, y_, w_, h_;
    const sf::Font* font_;   // pointer instead of reference — allows assignment

    std::vector<MazeButton> buttons_;
    SolveResult bfs_, dfs_;
    bool hasBFS_ = false, hasDFS_ = false;
    std::string status_   = "Ready";
    int mazeRows_ = 0, mazeCols_ = 0;

    void drawText(sf::RenderWindow& win, const std::string& str,
                  float tx, float ty, int size, sf::Color color);

    void drawResultBox(sf::RenderWindow& win, const SolveResult& r,
                       float bx, float by, float bw, sf::Color accent);
};