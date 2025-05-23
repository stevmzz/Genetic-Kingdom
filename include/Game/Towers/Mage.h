//
// Created by josep on 5/21/25.
//

// Mage.h
#ifndef MAGE_H
#define MAGE_H

#include "Tower.h"
#include <iostream>

class Mage : public Tower {
public:
    Mage();
    void attack(Enemy& enemy, const std::vector<std::unique_ptr<Enemy>>& allEnemies) override;
    std::string type() const override { return "Mage"; }
};

#endif // MAGE_H
