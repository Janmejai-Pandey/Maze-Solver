#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

// Renamed from Button to MazeButton to avoid conflict with sf::Mouse::Button
class MazeButton {
public:
    MazeButton(float x, float y, float w, float h,
               const std::string& label, const sf::Font& font);

    void setCallback(std::function<void()> cb) { callback_ = cb; }
    void setEnabled(bool e) { enabled_ = e; }
    void setToggled(bool t) { toggled_ = t; }
    bool isToggled() const  { return toggled_; }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape shape_;
    sf::Text           text_;
    std::function<void()> callback_;
    bool enabled_ = true;
    bool hovered_ = false;
    bool toggled_ = false;
};