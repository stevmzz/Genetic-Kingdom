//
// Created by josep on 5/21/25.
//

#ifndef TOWER_H
#define TOWER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class Enemy; // Forward declaration

class Tower {
protected:
    int damage; // que tanto dano hace la unidad
    float range; // rango al que puede atacar
    float attackSpeed; // intercalo de ataque en segundos
    float specialCooldown;
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Clock attackClock; // mide el tiempo desde el último ataque

public:
    Tower(int damage, float range, float attackSpeed, float specialCooldown)
        : damage(damage), range(range), attackSpeed(attackSpeed), specialCooldown(specialCooldown) {}

    virtual void attack(Enemy& enemy) = 0;
    virtual std::string type() const = 0;

    virtual ~Tower() = default;

    float getRange() const { return range; }

    virtual void setPosition(const sf::Vector2f& pos) {sprite.setPosition(pos);}

    virtual const sf::Sprite& getSprite() const {return sprite;}
};

#endif //TOWER_H
