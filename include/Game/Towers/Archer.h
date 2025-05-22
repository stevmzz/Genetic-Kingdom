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
    Archer() : Tower(25, 400.0f, 1.0f, 5.0f) {
        texture.loadFromFile("assets/images/towers/Archer.png");
        sprite.setTexture(texture);

        // Centrar el origen del sprite
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

        // Escalar el sprite (ajusta el tamaño a tu celda)
        float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
        sprite.setScale(scaleFactor, scaleFactor);

    }

    void attack(Enemy& enemy) override;
    std::string type() const override { return "Archer"; }
};

#endif // ARCHER_H
