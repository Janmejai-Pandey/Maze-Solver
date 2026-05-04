#include "UIPanel.h"
#include "Colors.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// ── Event handling ────────────────────────────────────────────
void UIPanel::handleEvent(const sf::Event& e, const sf::RenderWindow& w)
{
    for (auto& btn : buttons_)
        btn.handleEvent(e, w);
}

// ── Helper: draw a text string ────────────────────────────────
void UIPanel::drawText(sf::RenderWindow& win,
                       const std::string& str,
                       float tx, float ty,
                       int size, sf::Color color)
{
    if (!font_) return;
    sf::Text t(*font_, str, (unsigned int)size);
    t.setFillColor(color);
    t.setPosition({tx, ty});
    win.draw(t);
}

// ── Helper: draw one algorithm result box ─────────────────────
void UIPanel::drawResultBox(sf::RenderWindow& win,
                             const SolveResult& r,
                             float bx, float by,
                             float bw, sf::Color accent)
{
    // Box background
    sf::RectangleShape box;
    box.setPosition({bx, by});
    box.setSize({bw, 120.f});
    box.setFillColor(sf::Color(35, 35, 35));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(accent);
    win.draw(box);

    float ty = by + 5.f;

    // Algorithm name
    drawText(win, r.algorithmName, bx + 10.f, ty, 16, accent);
    ty += 22.f;

    if (r.found) {
        drawText(win, "Path found!", bx + 10.f, ty, 13,
                 sf::Color(100, 255, 100));
        ty += 18.f;

        std::ostringstream oss;

        oss << "Length: " << r.path.size() << " steps";
        drawText(win, oss.str(), bx + 10.f, ty, 12,
                 MazeColors::TEXT_SECONDARY);
        ty += 16.f;
        oss.str("");

        oss << "Explored: " << r.nodesExplored << " nodes";
        drawText(win, oss.str(), bx + 10.f, ty, 12,
                 MazeColors::TEXT_SECONDARY);
        ty += 16.f;
        oss.str("");

        oss << "Time: " << std::fixed << std::setprecision(2)
            << r.timeMs << " ms";
        drawText(win, oss.str(), bx + 10.f, ty, 12,
                 MazeColors::TEXT_SECONDARY);
    }
    else {
        drawText(win, "No path found!", bx + 10.f, ty, 13,
                 sf::Color(255, 100, 100));
    }
}

// ── Main draw ─────────────────────────────────────────────────
void UIPanel::draw(sf::RenderWindow& window)
{
    if (!font_) return;

    // Panel background
    sf::RectangleShape bg;
    bg.setPosition({x_, y_});
    bg.setSize({w_, h_});
    bg.setFillColor(MazeColors::PANEL_BG);
    window.draw(bg);

    // Title
    drawText(window, "MAZE SOLVER", x_ + 15.f, y_ + 10.f, 22,
             MazeColors::TEXT_PRIMARY);
    drawText(window, "BFS vs DFS",  x_ + 15.f, y_ + 36.f, 14,
             MazeColors::TEXT_SECONDARY);

    // Separator line
    sf::RectangleShape sep;
    sep.setPosition({x_ + 10.f, y_ + 58.f});
    sep.setSize({w_ - 20.f, 1.f});
    sep.setFillColor(sf::Color(80, 80, 80));
    window.draw(sep);

    // Status text
    drawText(window, "Status: " + status_,
             x_ + 15.f, y_ + 65.f, 13,
             sf::Color(200, 200, 100));

    // Grid info
    if (mazeRows_ > 0) {
        std::string info = "Grid: "
            + std::to_string(mazeRows_) + "x"
            + std::to_string(mazeCols_);
        drawText(window, info,
                 x_ + 15.f, y_ + 82.f, 12,
                 MazeColors::TEXT_SECONDARY);
    }

    // All buttons
    for (auto& btn : buttons_)
        btn.draw(window);

    // Result boxes (appear after buttons)
    float resultY = y_ + 360.f;

    if (hasBFS_) {
        drawResultBox(window, bfs_,
                      x_ + 10.f, resultY,
                      w_ - 20.f, MazeColors::BFS_PATH);
        resultY += 130.f;
    }

    if (hasDFS_) {
        drawResultBox(window, dfs_,
                      x_ + 10.f, resultY,
                      w_ - 20.f, MazeColors::DFS_PATH);
        resultY += 130.f;
    }

    // Winner comparison line
    if (hasBFS_ && hasDFS_ && bfs_.found && dfs_.found)
    {
        std::string winner;
        if (bfs_.path.size() < dfs_.path.size())
            winner = "BFS shorter: "
                + std::to_string(bfs_.path.size())
                + " vs "
                + std::to_string(dfs_.path.size());
        else if (dfs_.path.size() < bfs_.path.size())
            winner = "DFS shorter: "
                + std::to_string(dfs_.path.size())
                + " vs "
                + std::to_string(bfs_.path.size());
        else
            winner = "Equal: "
                + std::to_string(bfs_.path.size())
                + " steps";

        drawText(window, winner,
                 x_ + 15.f, resultY + 5.f, 11,
                 sf::Color(100, 255, 200));
    }

    // ── Legend ────────────────────────────────────────────────
    float legendY = h_ - 150.f;

    drawText(window, "Legend:",
             x_ + 15.f, y_ + legendY, 14,
             MazeColors::TEXT_PRIMARY);
    legendY += 20.f;

    auto drawLegendItem = [&](const std::string& label, sf::Color c)
    {
        sf::RectangleShape sq;
        sq.setPosition({x_ + 20.f, y_ + legendY + 2.f});
        sq.setSize({12.f, 12.f});
        sq.setFillColor(c);
        window.draw(sq);

        drawText(window, label,
                 x_ + 40.f, y_ + legendY, 12,
                 MazeColors::TEXT_SECONDARY);
        legendY += 18.f;
    };

    drawLegendItem("Wall",            MazeColors::WALL);
    drawLegendItem("Open Path",       MazeColors::PATH_OPEN);
    drawLegendItem("Start",           MazeColors::START);
    drawLegendItem("End",             MazeColors::END_PT);
    drawLegendItem("BFS Path",        MazeColors::BFS_PATH);
    drawLegendItem("DFS Path",        MazeColors::DFS_PATH);
    drawLegendItem("Explored (blue)", MazeColors::BFS_EXPLORED);
}