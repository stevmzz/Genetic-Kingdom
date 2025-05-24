#include "../include/Game/Grid/Cell.h"
#include "Game/Towers/Tower.h"

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
    if (tower) {
        sf::Vector2f center = shape.getPosition();
        center.x += shape.getSize().x / 2.f;
        center.y += shape.getSize().y / 2.f;

        tower->setPosition(center);
        window.draw(tower->getSprite());
    }

    if (tower) {
        sf::Text levelText;
        const sf::Font& font = tower->getFont();

        // Protección extra
        if (font.getInfo().family.empty()) return;

        levelText.setFont(font);
        levelText.setCharacterSize(14);

        // Color según el nivel
        switch (tower->getLevel()) {
            case 1: levelText.setFillColor(sf::Color::White); break;
            case 2: levelText.setFillColor(sf::Color::Magenta); break;
            case 3: levelText.setFillColor(sf::Color::Red); break;
            default: levelText.setFillColor(sf::Color::White); break; // fallback para nivel no esperado
        }

        levelText.setString("Lv." + std::to_string(tower->getLevel()));

        sf::FloatRect bounds = levelText.getLocalBounds();
        levelText.setOrigin(bounds.width / 2.f, 0.f);
        levelText.setPosition((shape.getPosition().x + 35.0f), shape.getPosition().y + 50.f);

        if (tower->recentlyUpgraded) {
            float t = tower->upgradeFlashClock.getElapsedTime().asSeconds();

            if (t > 1.0f) {
                tower->recentlyUpgraded = false; // termina el parpadeo
            } else {
                if (static_cast<int>(t * 5) % 2 == 0) {
                    window.draw(levelText); // parpadeo cada 0.2s
                }
                return; // evita que se dibuje doble
            }
        }

        window.draw(levelText); // dibujo normal si no está parpadeando
    }
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

// colocar una torre en la celda
void Cell::placeTower(std::shared_ptr<Tower> t) {
    tower = t;
}

// obtener la torre colocada
std::shared_ptr<Tower> Cell::getTower() const {
    return tower;
}

// verificar si la celda tiene una torre
bool Cell::hasTower() const {
    return tower != nullptr;
}

