#include "../include/Game/Grid/Cell.h"

// constructor de la celda
Cell::Cell(float x, float y, float size) : selected(false), pathCell(false) {
    // configurar la forma rectangular de la celda
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(size, size));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color(100, 100, 100));
}



// dibuja la celda en la ventana
void Cell::draw(sf::RenderWindow& window) {
    window.draw(shape);
}



// verifica si un punto esta dentro de la celda
bool Cell::contains(float x, float y) const {
    return shape.getGlobalBounds().contains(x, y);
}



// selecciona/deselecciona la celda
void Cell::setSelected(bool selected) {
    this->selected = selected;
    if (selected) {
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(2.0f);
    } else {
        shape.setOutlineColor(sf::Color(100, 100, 100));
        shape.setOutlineThickness(1.0f);
    }
}



// establece la textura de la celda
void Cell::setTexture(const sf::Texture* texture) {
    if (texture) {
        shape.setTexture(texture);
        // ajustar la textura para que coincida con el tamaño de la celda
        shape.setTextureRect(sf::IntRect(0, 0, texture->getSize().x, texture->getSize().y));
        // hacer la celda opaca cuando tiene textura
        shape.setFillColor(sf::Color::White);
    } else {
        shape.setTexture(nullptr);
        shape.setFillColor(sf::Color::Transparent);
    }
}



// establece si la celda forma parte del camino
void Cell::setIsPath(bool isPath) {
    pathCell = isPath;
}



// Verifica si la celda forma parte del camino
bool Cell::isPathCell() const {
    return pathCell;
}



// obtiene la posición de la celda
sf::Vector2f Cell::getPosition() const {
    return shape.getPosition();
}



// obtiene el tamaño de la celda
float Cell::getSize() const {
    return shape.getSize().x;
}