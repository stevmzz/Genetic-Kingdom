#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> callback;
    sf::Texture buttonTexture;
    sf::Texture buttonSelectedTexture;

    bool useTexture;
    bool isSelected;
    bool hovered = false;

public:
    Button(float x, float y, float width, float height,
           const sf::Font& font, const std::string& text,
           std::function<void()> callback);

    void setSelected(bool selected);
    bool getSelected() const;
    void activate();
    void draw(sf::RenderWindow& window);
    void setText(const std::string& text);
    void setTextures(const std::string& normalTexture, const std::string& selectedTexture);
    void setCallback(std::function<void()> callback);
    bool handleEvent(const sf::Event& event);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const {return shape.getPosition();}
};

#endif // BUTTON_H