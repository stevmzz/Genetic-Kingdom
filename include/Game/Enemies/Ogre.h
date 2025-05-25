#pragma once

#include "Enemy.h"
#include "../Genetics/Chromosome.h"

class Ogre : public Enemy {
public:
    Ogre(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path);
    Ogre(const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path, const Chromosome& chromosome);

    void update(float dt) override;
    void takeDamage(float amount, const std::string& damageType) override;
};