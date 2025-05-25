//
// Created by josep on 5/21/25.
//

#ifndef TOWER_H
#define TOWER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "./DataStructures/DynamicArray.h"

class Enemy; // Forward declaration

class Tower {
protected:
    int cost; // cuanto cuesta la unidad
    int damage; // que tanto dano hace la unidad
    float range; // rango al que puede atacar
    float attackSpeed; // intercalo de ataque en segundos
    float specialCooldown;
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Clock attackClock; // mide el tiempo desde el último ataque
    sf::Clock specialClock; // mide el tiempo desde el ultimo especial
    float specialChance = 0.8f; // chance de que el especial ocurra (80%)
    int level = 1;
    const int maxLevel = 3;
    inline static sf::Font sharedFont;

public:
    Tower(int cost, int damage, float range, float attackSpeed, float specialCooldown)
        : cost(cost), damage(damage), range(range), attackSpeed(attackSpeed), specialCooldown(specialCooldown) {}

    virtual void attack(Enemy& enemy, const DynamicArray<std::unique_ptr<Enemy>>& allEnemies) = 0;    virtual std::string type() const = 0;

    sf::Clock upgradeFlashClock;
    bool recentlyUpgraded = false;

    virtual ~Tower() = default;

    float getRange() const { return range; }
    int getCost() const { return cost; }

    virtual void setPosition(const sf::Vector2f& pos) {sprite.setPosition(pos);}

    virtual const sf::Sprite& getSprite() const {return sprite;}

    virtual bool canUpgrade() const { return level < maxLevel; }
    virtual int getUpgradeCost() const = 0;
    virtual void upgrade() = 0;
    int getLevel() const { return level; }

    static void setSharedFont(const sf::Font& font) {
        sharedFont = font;
    }

    const sf::Font& getFont() const {
        return sharedFont;
    }
};

#endif //TOWER_H
