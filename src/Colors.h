#pragma once
#include <SFML/Graphics.hpp>

namespace MazeColors {
    const sf::Color WALL          (40,  40,  40);
    const sf::Color PATH_OPEN     (240, 240, 240);
    const sf::Color START         (46,  204, 113);
    const sf::Color END_PT        (231, 76,  60);
    const sf::Color BFS_PATH      (241, 196, 15);
    const sf::Color DFS_PATH      (155, 89,  182);
    const sf::Color BFS_EXPLORED  (133, 193, 233);   // light blue
    const sf::Color DFS_EXPLORED  (174, 214, 241);   // lighter blue
    const sf::Color GRID_LINE     (60,  60,  60);
    const sf::Color BACKGROUND    (30,  30,  30);
    const sf::Color PANEL_BG      (45,  45,  45);
    const sf::Color TEXT_PRIMARY   (255, 255, 255);
    const sf::Color TEXT_SECONDARY (180, 180, 180);
    const sf::Color BTN_NORMAL    (52,  73,  94);
    const sf::Color BTN_HOVER     (41,  128, 185);
    const sf::Color BTN_ACTIVE    (39,  174, 96);
    const sf::Color BTN_DISABLED  (80,  80,  80);
    const sf::Color BTN_TEXT      (255, 255, 255);
}