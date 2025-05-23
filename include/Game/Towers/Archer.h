//
// Created by josep on 5/21/25.
//

// Archer.h
#ifndef ARCHER_H
#define ARCHER_H

#include "Tower.h"
#include <iostream>

class Archer : public Tower {
public:
    Archer();
    void attack(Enemy& enemy, const std::vector<std::unique_ptr<Enemy>>&) override;
    std::string type() const override { return "Archer"; }
    int getUpgradeCost() const override;
    void upgrade() override;

private:
    bool burstActive = false;
    int burstShotsFired = 0;
    int totalBurstShots = 3;
    float burstInterval = 0.2f; // 0.2 segundos entre tiros
    sf::Clock burstClock;
};

#endif // ARCHER_H
