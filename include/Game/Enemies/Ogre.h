#pragma once

#include "Enemy.h"

class Ogre : public Enemy {
private:
    float damageReduction;

public:
    Ogre(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path);

    void update(float dt) override;
    void takeDamage(float amount, const std::string& damageType) override;
};