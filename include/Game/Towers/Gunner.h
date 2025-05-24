//
// Created by josep on 5/21/25.
//

// Gunner.h
#ifndef GUNNER_H
#define GUNNER_H

#include "Tower.h"
#include <iostream>

#include "Effects/Cannonball.h"

class Gunner : public Tower {
public:
    Gunner();
    void attack(Enemy& enemy, const std::vector<std::unique_ptr<Enemy>>&) override;
    std::string type() const override { return "Gunner"; }
    int getUpgradeCost() const override;
    void upgrade() override;
    void updateProjectiles(float dt);
    void drawProjectiles(sf::RenderWindow& window);
private:
    sf::Texture bulletTexture;
    std::vector<Cannonball> activeBullets;
};

#endif // GUNNER_H
