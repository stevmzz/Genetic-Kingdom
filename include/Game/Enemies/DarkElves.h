#pragma once

#include "Enemy.h"
#include "../Genetics/Chromosome.h"

class DarkElves : public Enemy {
public:
    DarkElves(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path);
    DarkElves(const sf::Vector2f& position, const std::vector<sf::Vector2f>& path, const Chromosome& chromosome);

    void update(float dt) override;
    void takeDamage(float amount, const std::string& damageType) override;
};