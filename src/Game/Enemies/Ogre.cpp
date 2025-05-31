#include "../include/Game/Enemies/Ogre.h"
#include "../include/Game/Systems/Pathfinding.h"
#include "../include/DataStructures/DynamicArray.h"
#include <iostream>
#include <cmath>

// valores del ogro
const float OGRE_BASE_HEALTH = 160.0f;          // vida del ogro
const float OGRE_BASE_SPEED = 45.0f;            // velocidad baja del ogro
const float OGRE_ARROW_RESISTANCE = 0.6f;       // recistencia a flechas
const float OGRE_MAGIC_RESISTANCE = 1.5f;       // recistencia contra la magia
const float OGRE_ARTILLERY_RESISTANCE = 1.5f;   // recistencia contra la artillería



// constructor del ogro
Ogre::Ogre(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path)
    : Enemy(OGRE_BASE_HEALTH, OGRE_BASE_SPEED, OGRE_ARROW_RESISTANCE, OGRE_MAGIC_RESISTANCE, OGRE_ARTILLERY_RESISTANCE, 25, position, path){

    // cargar la textura del ogro
    if (!loadTexture("assets/images/enemies/ogre.png")) {
        std::cerr << "error al cargar imagen: ogro" << std::endl;
    }

    // ajustar el tamano del sprite del ogro
    sprite.setScale(0.3f, 0.3f); // reducir al 30% del tamano original

    // no girar el sprite inicialmente
    sprite.setRotation(0.0f);
}



// constructor con cromosoma
Ogre::Ogre(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path, const Chromosome& chromosome, int waveNumber)
    : Enemy(
    OGRE_BASE_HEALTH * (0.7f + (chromosome.getHealth() / 300.0f) * 0.6f) * std::pow(1.35f, static_cast<float>(waveNumber)),
    OGRE_BASE_SPEED * (0.8f + (chromosome.getSpeed() / 100.0f) * 0.4f) * std::pow(1.05f, static_cast<float>(waveNumber)),
    OGRE_ARROW_RESISTANCE * (0.9f + (chromosome.getArrowResistance() / 2.0f) * 0.2f) * std::pow(1.1f, static_cast<float>(waveNumber)),
    OGRE_MAGIC_RESISTANCE * (1.1f + (chromosome.getMagicResistance() / 2.0f) * 0.4f) * std::pow(1.1f, static_cast<float>(waveNumber)),
    OGRE_ARTILLERY_RESISTANCE * (1.1f + (chromosome.getArtilleryResistance() / 2.0f) * 0.4f) * std::pow(1.1f, static_cast<float>(waveNumber)),
    15 + static_cast<int>(std::pow(1.35f, waveNumber)), position, path) {

    // cargar la textura del ogro
    if (!loadTexture("assets/images/enemies/ogre.png")) {
        std::cerr << "error al cargar imagen: ogro" << std::endl;
    }

    // ajustar el tamaño del sprite del ogro
    sprite.setScale(0.4f, 0.4f);
    sprite.setRotation(0.0f);
}



// implementacion del metodo update
void Ogre::update(float dt) {
    Enemy::update(dt);
    if (!isActive || path.empty() || currentPathIndex >= path.size()) {
        return;
    }

    // posicion anterior para calcular distancia recorrida
    sf::Vector2f oldPosition = position;

    // usar el sistema de pathfinding para mover al ogro
    position = Pathfinding::moveAlongPath(position, path, currentPathIndex, speed, dt);

    // actualizar la posicion visual
    sprite.setPosition(position);

    // calcular distancia recorrida en este frame
    sf::Vector2f deltaPos = position - oldPosition;
    float distance = std::sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
    totalDistanceTraveled += distance;

    // obtener la direccion del movimiento
    if (currentPathIndex < path.size()) {
        sf::Vector2f nextPoint = path[currentPathIndex];
        sf::Vector2f moveDirection = Pathfinding::getDirection(position, nextPoint);
        direction = moveDirection;
    }
}



// implementacion del metodo para recibir dano
void Ogre::takeDamage(float amount, const std::string& damageType) {
    float damageMultiplier = 1.0f;

    // aplicar resistencias segun el tipo de dano
    if (damageType == "arrow") {
        damageMultiplier = arrowResistance; // resistente a flechas
    } else if (damageType == "magic") {
        damageMultiplier = magicResistance; // debil contra magia
    } else if (damageType == "artillery") {
        damageMultiplier = artilleryResistance; // debil contra artilleria
    }

    // aplicar la reduccion de dano especial del ogro
    float finalDamage = amount * damageMultiplier;

    // reducir la salud
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

    // verificar si el enemigo ha muerto
    if (health <= 0) {
        health = 0;
        isActive = false;
    }
}