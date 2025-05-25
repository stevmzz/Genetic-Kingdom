//
// Created by josep on 5/21/25.
//

// Mage.h
#ifndef MAGE_H
#define MAGE_H

#include "Tower.h"
#include <iostream>
#include "Effects/Fireball.h"
#include <vector>

class Mage : public Tower {
public:
    Mage();
    void attack(Enemy& enemy, const DynamicArray<std::unique_ptr<Enemy>>& allEnemies) override;
    std::string type() const override { return "Mage"; }
    int getUpgradeCost() const override;
    void upgrade() override;
    void updateProjectiles(float dt);
    void drawProjectiles(sf::RenderWindow& window);
private:
    std::vector<Fireball> activeFireballs;
    sf::Texture fireballTexture;
};

#endif // MAGE_H
