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

int Gunner::getUpgradeCost() const {
    return 100 + (level * 100); // Nivel 1:200, 2:300
}

void Gunner::upgrade() {
    if (canUpgrade()) {
        level++;
        damage += 30;            // gran aumento de daño
        range += 15.0f;          // mejora leve en rango
        attackSpeed += 0.1f;     // mejora mínima en cadencia
        specialChance += 0.1f;   // aumenta probabilidad de especial

        recentlyUpgraded = true;
        upgradeFlashClock.restart();

        std::cout << "Gunner upgraded to level " << level << "\n";
    }
}