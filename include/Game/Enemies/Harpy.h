#pragma once

#include "Enemy.h"
#include "../Genetics/Chromosome.h"

class Harpy : public Enemy {
public:
    Harpy(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path);
    Harpy(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path, const Chromosome& chromosome);

    void update(float dt) override;
    void takeDamage(float amount, const std::string& damageType) override;
};