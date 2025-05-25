#include "../include/Game/Enemies/DarkElves.h"
#include "../include/Game/Systems/Pathfinding.h"
#include "../include/DataStructures/DynamicArray.h"
#include <iostream>
#include <cmath>

// valores base del elfo oscuro
const float DARKELF_BASE_HEALTH = 100.0f;             // vida menor
const float DARKELF_BASE_SPEED = 120.0f;              // muy rápido
const float DARKELF_ARROW_RESISTANCE = 0.1f;          // débil contra flechas
const float DARKELF_MAGIC_RESISTANCE = 0.8f;          // resistente a la magia
const float DARKELF_ARTILLERY_RESISTANCE = 0.2f;      // débil contra artillería

// constructor base
DarkElves::DarkElves(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path)
    : Enemy(DARKELF_BASE_HEALTH, DARKELF_BASE_SPEED, DARKELF_ARROW_RESISTANCE, DARKELF_MAGIC_RESISTANCE, DARKELF_ARTILLERY_RESISTANCE, 15, position, path) {

    if (!loadTexture("assets/images/enemies/DarkElve.png")) {
        std::cerr << "error al cargar imagen: elfo oscuro" << std::endl;
    }

    sprite.setScale(0.11f, 0.11f); // tamaño intermedio
    sprite.setRotation(0.0f);
}

// constructor con cromosoma (algoritmo genético)
DarkElves::DarkElves(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path, const Chromosome& chromosome)
    : Enemy(
        DARKELF_BASE_HEALTH * (0.7f + (chromosome.getHealth() / 300.0f) * 0.6f),
        DARKELF_BASE_SPEED * (0.8f + (chromosome.getSpeed() / 100.0f) * 0.4f),
        DARKELF_ARROW_RESISTANCE * (0.8f + (chromosome.getArrowResistance() / 2.0f) * 0.4f),
        DARKELF_MAGIC_RESISTANCE * (0.8f + (chromosome.getMagicResistance() / 2.0f) * 0.4f),
        DARKELF_ARTILLERY_RESISTANCE * (0.8f + (chromosome.getArtilleryResistance() / 2.0f) * 0.4f),
        15, position, path) {

    if (!loadTexture("assets/images/enemies/DarkElve.png")) {
        std::cerr << "error al cargar imagen: elfo oscuro" << std::endl;
    }

    sprite.setScale(0.11f, 0.11f);
    sprite.setRotation(0.0f);
}

// actualización por frame
void DarkElves::update(float dt) {
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
void DarkElves::takeDamage(float amount, const std::string& damageType) {
    float damageMultiplier = 1.0f;

    if (damageType == "arrow") {
        damageMultiplier = 1.0f - arrowResistance; // débil a flechas
    } else if (damageType == "magic") {
        damageMultiplier = magicResistance; // resistente a magia
    } else if (damageType == "artillery") {
        damageMultiplier = 1.0f - artilleryResistance; // débil a artillería
    }

    float finalDamage = amount * damageMultiplier;
    health -= finalDamage;

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
