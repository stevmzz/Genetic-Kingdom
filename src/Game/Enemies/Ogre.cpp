#include "../include/Game/Enemies/Ogre.h"
#include "../include/Game/Systems/Pathfinding.h"
#include <iostream>
#include <cmath>

// valores del ogro
const float OGRE_BASE_HEALTH = 200.0f;          // vida del ogro
const float OGRE_BASE_SPEED = 50.0f;            // velocidad baja del ogro
const float OGRE_ARROW_RESISTANCE = 0.7f;       // recistencia a flechas
const float OGRE_MAGIC_RESISTANCE = 1.5f;       // recistencia contra la magia
const float OGRE_ARTILLERY_RESISTANCE = 1.5f;   // recistencia contra la artillería



// constructor del ogro
Ogre::Ogre(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path)
    : Enemy(OGRE_BASE_HEALTH, OGRE_BASE_SPEED, OGRE_ARROW_RESISTANCE, OGRE_MAGIC_RESISTANCE, OGRE_ARTILLERY_RESISTANCE, 15, position, path){

    // cargar la textura del ogro
    if (!loadTexture("assets/images/enemies/ogre.png")) {
        std::cerr << "error al cargar imagen: ogro" << std::endl;
    }

    // ajustar el tamano del sprite del ogro
    sprite.setScale(0.5f, 0.5f); // reducir a la mitad del tamano original

    // no girar el sprite inicialmente
    sprite.setRotation(0.0f);
}



// constructor con cromosoma
Ogre::Ogre(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path, const Chromosome& chromosome)
    : Enemy(
        // ajustar la salud: entre 70%-130% de la salud base del ogro
        OGRE_BASE_HEALTH * (0.7f + (chromosome.getHealth() / 300.0f) * 0.6f),

        // ajustar la velocidad: entre 80%-120% de la velocidad base del ogro
        OGRE_BASE_SPEED * (0.8f + (chromosome.getSpeed() / 100.0f) * 0.4f),

        // ajustar resistencia a flechas
        OGRE_ARROW_RESISTANCE * (0.8f + (chromosome.getArrowResistance() / 2.0f) * 0.4f),

        // ajustar debilidad a magia
        OGRE_MAGIC_RESISTANCE * (0.8f + (chromosome.getMagicResistance() / 2.0f) * 0.4f),

        // ajustar debilidad a artillería
        OGRE_ARTILLERY_RESISTANCE * (0.8f + (chromosome.getArtilleryResistance() / 2.0f) * 0.4f),

        15, position, path) {

    // cargar la textura del ogro
    if (!loadTexture("assets/images/enemies/ogre.png")) {
        std::cerr << "Error al cargar imagen: ogro" << std::endl;
    }

    // ajustar el tamaño del sprite del ogro
    sprite.setScale(0.5f, 0.5f);
    sprite.setRotation(0.0f);
}



// implementacion del metodo update
void Ogre::update(float dt) {
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
        damageMultiplier = 1.0f - arrowResistance; // resistente a flechas
    } else if (damageType == "magic") {
        damageMultiplier = magicResistance; // debil contra magia
    } else if (damageType == "artillery") {
        damageMultiplier = artilleryResistance; // debil contra artilleria
    }

    // aplicar la reduccion de dano especial del ogro
    float finalDamage = amount * damageMultiplier;

    // reducir la salud
    health -= finalDamage;

    // verificar si el enemigo ha muerto
    if (health <= 0) {
        health = 0;
        isActive = false;
    }
}