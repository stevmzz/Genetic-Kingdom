#include "../include/Game/Enemies/Harpy.h"
#include "../include/Game/Systems/Pathfinding.h"
#include "../include/DataStructures/DynamicArray.h"
#include <iostream>
#include <cmath>

// valores base de la harpía
const float HARPY_BASE_HEALTH = 90.0f;              // vida media
const float HARPY_BASE_SPEED = 75.0f;                // velocidad intermedia
const float HARPY_ARROW_RESISTANCE = 1.0f;           // resistencia neutra
const float HARPY_MAGIC_RESISTANCE = 1.0f;           // resistencia neutra
const float HARPY_ARTILLERY_RESISTANCE = 0.0f;       // inmunidad: no se puede atacar con artillería

// constructor normal
Harpy::Harpy(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path)
    : Enemy(HARPY_BASE_HEALTH, HARPY_BASE_SPEED, HARPY_ARROW_RESISTANCE, HARPY_MAGIC_RESISTANCE, HARPY_ARTILLERY_RESISTANCE, 30, position, path) {

    if (!loadTexture("assets/images/enemies/Harpy.png")) {
        std::cerr << "error al cargar imagen: harpía" << std::endl;
    }

    sprite.setScale(0.11f, 0.11f); // tamaño proporcional
    sprite.setRotation(0.0f);
}

// constructor con cromosoma
Harpy::Harpy(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path, const Chromosome& chromosome, int waveNumber)
    : Enemy(
    HARPY_BASE_HEALTH * (0.7f + (chromosome.getHealth() / 300.0f) * 0.6f) * std::pow(1.35f, static_cast<float>(waveNumber)),
    HARPY_BASE_SPEED * (0.8f + (chromosome.getSpeed() / 100.0f) * 0.4f) * std::pow(1.05f, static_cast<float>(waveNumber)),
    HARPY_ARROW_RESISTANCE * (0.9f + (chromosome.getArrowResistance() / 2.0f) * 0.2f) * std::pow(1.1f, static_cast<float>(waveNumber)),
    HARPY_MAGIC_RESISTANCE * (1.1f + (chromosome.getMagicResistance() / 2.0f) * 0.4f) * std::pow(1.1f, static_cast<float>(waveNumber)),
    HARPY_ARTILLERY_RESISTANCE, // no escala, sigue inafectada por artillería
    15 + static_cast<int>(std::pow(1.35f, waveNumber)), position, path) {

    if (!loadTexture("assets/images/enemies/Harpy.png")) {
        std::cerr << "error al cargar imagen: harpía" << std::endl;
    }

    sprite.setScale(0.15f, 0.15f);
    sprite.setRotation(0.0f);
}

// actualización por frame
void Harpy::update(float dt) {
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

// recibir daño según tipo
void Harpy::takeDamage(float amount, const std::string& damageType) {
    float damageMultiplier = 0.0f;

    if (damageType == "arrow") {
        damageMultiplier = arrowResistance;
    } else if (damageType == "magic") {
        damageMultiplier = magicResistance;
    } else if (damageType == "artillery") {
        damageMultiplier = artilleryResistance; // inmunidad a artillería
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
