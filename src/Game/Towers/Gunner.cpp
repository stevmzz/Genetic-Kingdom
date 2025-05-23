//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Gunner.h"
#include "Game/Enemies/Enemy.h"

Gunner::Gunner() : Tower(
    150, // cost
    90, // damage
    180.0f, // range
    0.5f, // attack speed
    8.0f // special cooldown
    ) {
    texture.loadFromFile("assets/images/towers/Gunner.png");
    sprite.setTexture(texture);

    // Centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Escalar el sprite (ajusta el tamaño a tu celda)
    float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
    sprite.setScale(scaleFactor, scaleFactor);
}

void Gunner::attack(Enemy& enemy, const std::vector<std::unique_ptr<Enemy>>&) {
    float elapsed = attackClock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f / attackSpeed) {
        enemy.receiveDamage(damage);
        attackClock.restart(); // reinicia el temporizador
    }

    // ataque especial de alto daño
    float specialElapsed = specialClock.getElapsedTime().asSeconds();
    if (specialElapsed >= specialCooldown) {
        float roll = static_cast<float>(rand()) / RAND_MAX;
        if (roll <= specialChance) {
            float specialDamage = damage * 2.0f;
            std::cout << "Gunner fires big damage shot causing " << specialDamage << " damage\n";
            enemy.receiveDamage(specialDamage);
        }
        specialClock.restart();
    }
}