#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "../include//Game/Grid/Cell.h"

class Grid {
public:
    Grid(float x, float y, int rows, int cols, float cellSize);
    void draw(sf::RenderWindow& window);
    Cell* getCellAtPosition(float x, float y);
    void clearSelection();

private:
    std::vector<std::vector<Cell>> cells;
    float x, y;
    float cellSize;
    int rows, cols;
};