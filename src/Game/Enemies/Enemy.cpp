#include "../include/Game/Enemies/Enemy.h"
#include "../include/Game/Systems/Pathfinding.h"
#include <cmath>
#include <iostream>

#include "Game/Towers/Tower.h"

// constructor del enemigo
Enemy::Enemy(float health, float speed, float arrowRes, float magicRes, float artilleryRes, int goldReward, const sf::Vector2f& position, const std::vector<sf::Vector2f>& path)
    :   id(-1),
        health(health),
        maxHealth(health),
        speed(speed),
        arrowResistance(arrowRes),
        magicResistance(magicRes),
        artilleryResistance(artilleryRes),
        goldReward(goldReward),
        position(position),
        isActive(true),
        path(path),
        currentPathIndex(0),
        totalDistanceTraveled(0.0f) {

    // inicializar direccion si hay un camino
    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex + 1]);
    }

    // iniciar el temporizador de vida
    lifeTimer.restart();
}



// contructor badado en cromosoma
Enemy::Enemy(const Chromosome& chromosome, int goldReward, const sf::Vector2f& position, const std::vector<sf::Vector2f>& path)
    :   id(-1),
        health(chromosome.getHealth()),
        maxHealth(chromosome.getHealth()),
        speed(chromosome.getSpeed()),
        arrowResistance(chromosome.getArrowResistance()),
        magicResistance(chromosome.getMagicResistance()),
        artilleryResistance(chromosome.getArtilleryResistance()),
        goldReward(goldReward),
        position(position),
        isActive(true),
        path(path),
        currentPathIndex(0),
        totalDistanceTraveled(0.0f) {

    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex + 1]);
    }

    lifeTimer.restart();
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
        healthBar.setPosition(position.x - 20.f, position.y - 40.f);
        target.draw(healthBar, states);

    }

    // dibujar los textos flotantes de daño
    for (const auto& dmgText : floatingTexts) {
        target.draw(dmgText.text, states);
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



// establecer id
void Enemy::setId(int id) {
    this->id = id;
}



// obtener id
int Enemy::getId() const {
    return id;
}



// obtener la distancia total recorrida
float Enemy::getTotalDistanceTraveled() const {
    return totalDistanceTraveled;
}



// obtener el tiempo vivo
float Enemy::getTimeAlive() const {
    return lifeTimer.getElapsedTime().asSeconds();
}

// aplicar daño al enemigo
void Enemy::receiveDamage(float damage) {
    // reducir salud
    health -= damage;

    // Crear texto flotante
    FloatingDamageText damageText;
    damageText.text.setFont(sharedFont);
    damageText.text.setCharacterSize(16);
    damageText.text.setFillColor(sf::Color::Red);
    damageText.text.setString("-" + std::to_string(static_cast<int>(damage)));

    sf::FloatRect bounds = damageText.text.getLocalBounds();
    damageText.text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Posición encima del enemigo
    damageText.text.setPosition(position.x, position.y - 60.f);

    floatingTexts.push_back(damageText);

    // si la salud cae por debajo de 0, marcar como inactivo
    if (health <= 0.f) {
        health = 0.f;
        isActive = false;
    }
}

void Enemy::setSharedFont(const sf::Font& font) {
    sharedFont = font;
}

void Enemy::update(float dt) {
    for (auto it = floatingTexts.begin(); it != floatingTexts.end(); ) {
        float t = it->timer.getElapsedTime().asSeconds();

        if (t > 0.8f) {
            it = floatingTexts.erase(it);
        } else {
            sf::Vector2f pos = it->text.getPosition();
            pos.y -= 20.f * dt; // flota hacia arriba
            it->text.setPosition(pos);

            // desvanecer
            sf::Color color = it->text.getFillColor();
            color.a = static_cast<sf::Uint8>(255 * (1.0f - t / 0.8f));
            it->text.setFillColor(color);

            ++it;
        }
    }
}
