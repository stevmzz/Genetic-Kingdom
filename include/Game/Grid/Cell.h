#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "Game/Towers/Tower.h"

class Cell {
public:
    Cell(float x, float y, float size);
    void draw(sf::RenderWindow& window);
    bool contains(float x, float y) const;
    void setSelected(bool selected);
    void setTexture(const sf::Texture* texture);
    void setIsPath(bool isPath);
    bool isPathCell() const;
    sf::Vector2f getPosition() const;
    float getSize() const;
    void placeTower(std::shared_ptr<Tower> t);
    std::shared_ptr<Tower> getTower() const;
    bool hasTower() const;

private:
    sf::RectangleShape shape;
    bool selected;
    bool pathCell;
    std::shared_ptr<Tower> tower;
};