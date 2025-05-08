#include "../include/Game/Grid/Grid.h"



// constructor de la cuadrícula
Grid::Grid(float x, float y, int rows, int cols, float cellSize)
    : x(x), y(y), rows(rows), cols(cols), cellSize(cellSize) {

    // inicializar la matriz de celdas
    cells.resize(rows);
    for (int i = 0; i < rows; i++) {
        cells[i].reserve(cols);
        for (int j = 0; j < cols; j++) {
            cells[i].emplace_back(x + j * cellSize, y + i * cellSize, cellSize);
        }
    }
}



// dibuja la cuadrícula en la ventana
void Grid::draw(sf::RenderWindow& window) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cells[i][j].draw(window);
        }
    }
}



// obtiene la celda en las coordenadas absolutas de la ventana
Cell* Grid::getCellAtPosition(float x, float y) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (cells[i][j].contains(x, y)) {
                return &cells[i][j];
            }
        }
    }
    return nullptr;
}



// deselecciona todas las celdas
void Grid::clearSelection() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cells[i][j].setSelected(false);
        }
    }
}



// obtiene la celda en la posición de fila/columna especificada
Cell* Grid::getCellAt(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return &cells[row][col];
    }
    return nullptr;
}