#pragma once

#include <SFML/Graphics.hpp>

class Cell {
public:
    Cell(float x, float y, float size);
    void draw(sf::RenderWindow& window);
    bool contains(float x, float y) const;
    void setSelected(bool selected);
    void setTexture(const sf::Texture* texture);
    sf::Vector2f getPosition() const;
    float getSize() const;

private:
    sf::RectangleShape shape;
    bool selected;
};