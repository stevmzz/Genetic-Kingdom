#include "../include/Game/Enemies/Ogre.h"
#include "../include/Game/Systems/Pathfinding.h"
#include <iostream>
#include <cmath>

// constructor del ogro
Ogre::Ogre(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path)
    : Enemy(200.0f, 50.0f, 0.7f, 1.5f, 1.5f, 15, position, path){

    // cargar la textura del ogro
    if (!loadTexture("assets/images/enemies/ogre.png")) {
        std::cerr << "error al cargar imagen: ogro" << std::endl;
    }

    // ajustar el tamano del sprite del ogro
    sprite.setScale(0.5f, 0.5f); // reducir a la mitad del tamano original

    // no girar el sprite inicialmente
    sprite.setRotation(0.0f);
}



// implementacion del metodo update
void Ogre::update(float dt) {
    if (!isActive || path.empty() || currentPathIndex >= path.size()) {
        return;
    }

    // usar el sistema de pathfinding para mover al ogro
    position = Pathfinding::moveAlongPath(position, path, currentPathIndex, speed, dt);

    // actualizar la posicion visual
    sprite.setPosition(position);

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
    float finalDamage = amount * damageMultiplier * (1.0f - damageReduction);

    // reducir la salud
    health -= finalDamage;

    // verificar si el enemigo ha muerto
    if (health <= 0) {
        health = 0;
        isActive = false;
    }
}