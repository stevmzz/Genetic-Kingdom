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
    Gunner() : Tower(75, 200.0f, 0.5f, 7.0f) {
        texture.loadFromFile("assets/images/towers/Gunner.png");
        sprite.setTexture(texture);

        // Centrar el origen del sprite
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

        // Escalar el sprite (ajusta el tamaño a tu celda)
        float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
        sprite.setScale(scaleFactor, scaleFactor);
    }

    void attack(Enemy& enemy) override;
    std::string type() const override { return "Gunner"; }
};

#endif // GUNNER_H
