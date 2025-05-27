#include "../include/Game/Enemies/Mercenary.h"
#include "../include/Game/Systems/Pathfinding.h"
#include "../include/DataStructures/DynamicArray.h"
#include <iostream>
#include <cmath>

// valores base del mercenario
const float MERCENARY_BASE_HEALTH = 130.0f;             // vida alta
const float MERCENARY_BASE_SPEED = 60.0f;               // velocidad media
const float MERCENARY_ARROW_RESISTANCE = 0.6f;          // resistencia a flechas
const float MERCENARY_MAGIC_RESISTANCE = 1.3f;          // débil a magia
const float MERCENARY_ARTILLERY_RESISTANCE = 0.7f;      // resistencia a artillería

// constructor base
Mercenary::Mercenary(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path)
    : Enemy(MERCENARY_BASE_HEALTH, MERCENARY_BASE_SPEED, MERCENARY_ARROW_RESISTANCE, MERCENARY_MAGIC_RESISTANCE, MERCENARY_ARTILLERY_RESISTANCE, 25, position, path) {

    if (!loadTexture("assets/images/enemies/mercenary.png")) {
        std::cerr << "error al cargar imagen: mercenario" << std::endl;
    }

    sprite.setScale(0.11f, 0.11f);
    sprite.setRotation(0.0f);
}

// constructor con cromosoma
Mercenary::Mercenary(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path, const Chromosome& chromosome)
    : Enemy(
        MERCENARY_BASE_HEALTH * (0.7f + (chromosome.getHealth() / 300.0f) * 0.6f),
        MERCENARY_BASE_SPEED * (0.8f + (chromosome.getSpeed() / 100.0f) * 0.4f),
        MERCENARY_ARROW_RESISTANCE * (0.8f + (chromosome.getArrowResistance() / 2.0f) * 0.4f),
        MERCENARY_MAGIC_RESISTANCE * (0.8f + (chromosome.getMagicResistance() / 2.0f) * 0.4f),
        MERCENARY_ARTILLERY_RESISTANCE * (0.8f + (chromosome.getArtilleryResistance() / 2.0f) * 0.4f),
        15, position, path) {

    if (!loadTexture("assets/images/enemies/Mercenary.png")) {
        std::cerr << "error al cargar imagen: Mercenario" << std::endl;
    }

    sprite.setScale(0.11f, 0.11f);
    sprite.setRotation(0.0f);
}

// actualización por frame
void Mercenary::update(float dt) {
    Enemy::update(dt);
    if (!isActive || path.empty() || currentPathIndex >= path.size()) {
        return;
    }

    sf::Vector2f oldPosition = position;
    position = Pathfinding::moveAlongPath(position, path, currentPathIndex, speed, dt);
    sprite.setPosition(position);

    sf::Vector2f deltaPos = position - oldPosition;
    float distance = std::sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
    totalDistanceTraveled += distance;

    if (currentPathIndex < path.size()) {
        sf::Vector2f nextPoint = path[currentPathIndex];
        sf::Vector2f moveDirection = Pathfinding::getDirection(position, nextPoint);
        direction = moveDirection;
    }
}

// recibir daño
void Mercenary::takeDamage(float amount, const std::string& damageType) {
    float damageMultiplier = 1.0f;

    if (damageType == "arrow") {
        damageMultiplier = 1.0f - arrowResistance;
    } else if (damageType == "magic") {
        damageMultiplier = magicResistance;
    } else if (damageType == "artillery") {
        damageMultiplier = 1.0f - artilleryResistance;
    }

    float finalDamage = amount * damageMultiplier;
    Enemy::receiveDamage(finalDamage);

    // Crear texto flotante
    FloatingDamageText damageText;
    damageText.text.setFont(sharedFont);
    damageText.text.setCharacterSize(16);
    damageText.text.setFillColor(sf::Color::Red);
    damageText.text.setString("-" + std::to_string(static_cast<int>(finalDamage)));

    sf::FloatRect bounds = damageText.text.getLocalBounds();
    damageText.text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Posición encima del enemigo
    damageText.text.setPosition(position.x, position.y - 60.f);

    floatingTexts.push_back(damageText);

    if (health <= 0) {
        health = 0;
        isActive = false;
    }
}
