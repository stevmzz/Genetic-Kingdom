//
// Created by josep on 5/21/25.
//

// Gunner.h
#ifndef GUNNER_H
#define GUNNER_H

#include "Tower.h"
#include <iostream>

class Gunner : public Tower {
public:
    Gunner();
    void attack(Enemy& enemy, const std::vector<std::unique_ptr<Enemy>>&) override;
    std::string type() const override { return "Gunner"; }
    int getUpgradeCost() const override;
    void upgrade() override;
};

#endif // GUNNER_H
