#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "../include/Game/Grid/Cell.h"
#include "../include/DataStructures/DynamicArray.h"

class Grid {
public:
    Grid(float x, float y, int rows, int cols, float cellSize);
    void draw(sf::RenderWindow& window);
    Cell* getCellAtPosition(float x, float y);
    void clearSelection();
    Cell* getCellAt(int row, int col);
    float getX() const { return x; }
    float getY() const { return y; }
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    float getCellSize() const { return cellSize; }
    const DynamicArray<DynamicArray<Cell>>& getCells() const { return cells; }
    bool loadBackgroundTextures();
    sf::Texture groundTexture1;
    sf::Texture groundTexture2;
    bool texturesLoaded;

private:
    DynamicArray<DynamicArray<Cell>> cells;
    float x, y;
    float cellSize;
    int rows, cols;
};