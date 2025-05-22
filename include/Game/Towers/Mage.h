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
    Mage() : Tower(50, 300.0f, 0.8f, 6.0f) {
        texture.loadFromFile("assets/images/towers/Mage.png");
        sprite.setTexture(texture);

        // Centrar el origen del sprite
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

        // Escalar el sprite (ajusta el tamaño a tu celda)
        float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
        sprite.setScale(scaleFactor, scaleFactor);
    }

    void attack(Enemy& enemy) override;
    std::string type() const override { return "Mage"; }
};

#endif // MAGE_H
