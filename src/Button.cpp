#include "Button.h"
#include "Colors.h"

MazeButton::MazeButton(float x, float y, float w, float h,
                       const std::string& label, const sf::Font& font)
    : text_(font, label, 14)   // SFML 3: font passed in constructor
{
    shape_.setPosition({x, y});           // SFML 3: Vector2f
    shape_.setSize({w, h});
    shape_.setFillColor(MazeColors::BTN_NORMAL);
    shape_.setOutlineThickness(1.f);
    shape_.setOutlineColor(sf::Color(80, 80, 80));

    text_.setFillColor(MazeColors::BTN_TEXT);

    // Center text inside button
    sf::FloatRect tb = text_.getLocalBounds();
    text_.setOrigin({tb.position.x + tb.size.x / 2.f,
                     tb.position.y + tb.size.y / 2.f});
    text_.setPosition({x + w / 2.f, y + h / 2.f});
}

void MazeButton::handleEvent(const sf::Event& event,
                              const sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect bounds  = shape_.getGlobalBounds();
    hovered_ = bounds.contains({(float)mousePos.x, (float)mousePos.y});

    // SFML 3 event system
    if (const auto* e = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (e->button == sf::Mouse::Button::Left && hovered_ && enabled_) {
            if (callback_) callback_();
        }
    }
}

void MazeButton::draw(sf::RenderWindow& window) {
    if (!enabled_) {
        shape_.setFillColor(MazeColors::BTN_DISABLED);
    } else if (toggled_) {
        shape_.setFillColor(MazeColors::BTN_ACTIVE);
    } else if (hovered_) {
        shape_.setFillColor(MazeColors::BTN_HOVER);
    } else {
        shape_.setFillColor(MazeColors::BTN_NORMAL);
    }
    window.draw(shape_);
    window.draw(text_);
}