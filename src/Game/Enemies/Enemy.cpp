#include "../include/Game/Enemies/Enemy.h"
#include "../include/Game/Systems/Pathfinding.h"
#include <cmath>

// constructor del enemigo
Enemy::Enemy(float health, float speed,
             float arrowRes, float magicRes, float artilleryRes,
             int goldReward,
             const sf::Vector2f& position,
             const std::vector<sf::Vector2f>& path)
    : health(health), maxHealth(health), speed(speed),
      arrowResistance(arrowRes), magicResistance(magicRes), artilleryResistance(artilleryRes),
      goldReward(goldReward), position(position), isActive(true), path(path), currentPathIndex(0) {

    // inicializar direccion si hay un camino
    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex + 1]);
    }
}



// cargar textura
bool Enemy::loadTexture(const std::string& filename) {
    if (!texture.loadFromFile(filename)) {
        return false;
    }

    sprite.setTexture(texture);

    // centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2, bounds.height / 2);
    sprite.setPosition(position);

    return true;
}



// comprobar si el enemigo esta vivo
bool Enemy::isAlive() const {
    return health > 0.f && isActive;
}



// comprobar si ha llegado al final del camino
bool Enemy::hasReachedEnd() const {
    return Pathfinding::hasReachedEnd(currentPathIndex, path);
}



// obtener la recompensa de oro
int Enemy::getGoldReward() const {
    return goldReward;
}



// obtener la posicion actual
sf::Vector2f Enemy::getPosition() const {
    return position;
}



// dibujar el enemigo
void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (isActive) {
        target.draw(sprite, states);

        // dibujar la barra de vida
        sf::RectangleShape healthBar;
        healthBar.setSize(sf::Vector2f(40.f * (health / maxHealth), 5.f));
        healthBar.setFillColor(sf::Color::Green);
        healthBar.setPosition(position.x - 20.f, position.y - 65.f);
        target.draw(healthBar, states);
    }
}



// establecer un nuevo camino
void Enemy::setPath(const std::vector<sf::Vector2f>& newPath) {
    path = newPath;
    currentPathIndex = 0;

    // actualizar direccion
    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex]);
    }
}