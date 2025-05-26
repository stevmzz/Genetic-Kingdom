//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Archer.h"
#include "Game/Enemies/Enemy.h"
#include "Core/AudioSystem.h"


Archer::Archer() : Tower(
    75, // cost
    35, // damage
    280.0f, // range
    1.8f, // attack speed
    6.0f // special cooldown
    ) {
    texture.loadFromFile("assets/images/towers/Archer.png"); // textura de torre
    sprite.setTexture(texture);

    // Textura de flecha
    arrowTexture.loadFromFile("assets/images/towers/Arrow.png");

    // Centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Escalar el sprite (ajusta el tamaño a tu celda)
    float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
    sprite.setScale(scaleFactor, scaleFactor);

}

void Archer::attack(Enemy& enemy, const DynamicArray<std::unique_ptr<Enemy>>&) {
    // ataque normal
    float elapsed = attackClock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f / attackSpeed) {
        enemy.takeDamage(damage, "arrow");

        // sonido de flecha
        if (audioSystem) {
            audioSystem->playSound("arrow");
        }

        // Crear flecha
        Arrow arrow(arrowTexture, sprite.getPosition(), enemy.getPosition());
        activeArrows.push_back(arrow);

        attackClock.restart(); // reinicia el temporizador
    }

    // ataque especial con probabilidad
    // el ataque especial es una rafaga rapida de 3 tiros a un enemigo
    float specialElapsed = specialClock.getElapsedTime().asSeconds();
    if (!burstActive && specialElapsed >= specialCooldown) {
        float roll = static_cast<float>(rand()) / RAND_MAX;
        if (roll <= specialChance) {
            std::cout << "Archer fires a burst attack\n";
            burstActive = true;
            burstShotsFired = 0;
            burstClock.restart();
        }
        specialClock.restart();
    }

    // ejecutar ráfaga si está activa
    if (burstActive && burstShotsFired < totalBurstShots) {
        if (burstClock.getElapsedTime().asSeconds() >= burstInterval) {
            std::cout << "Burst shot " << (burstShotsFired + 1) << " hits for " << damage*0.8 << " damage.\n";
            enemy.takeDamage(damage*0.8, "arrow");

            // sonido de flecha
            if (audioSystem) {
                audioSystem->playSound("arrow");
            }

            // Crear flecha
            Arrow arrow(arrowTexture, sprite.getPosition(), enemy.getPosition());
            activeArrows.push_back(arrow);

            burstShotsFired++;
            burstClock.restart();
        }
    }

    // terminar la ráfaga
    if (burstActive && burstShotsFired >= totalBurstShots) {
        burstActive = false;
    }
}

int Archer::getUpgradeCost() const {
    return 60 + (level * 40);
}

void Archer::upgrade() {
    if (canUpgrade()) {
        level++;
        damage += 12;
        range += 25.0f;
        attackSpeed += 0.15f;

        recentlyUpgraded = true;
        upgradeFlashClock.restart();

        std::cout << "Archer upgraded to level " << level << "\n";
    }
}

void Archer::updateProjectiles(float dt) {
    for (auto it = activeArrows.begin(); it != activeArrows.end(); ) {
        it->update(dt);
        if (it->hasReachedTarget()) {
            it = activeArrows.erase(it); // eliminar flechas que ya llegaron
        } else {
            ++it;
        }
    }
}

void Archer::drawProjectiles(sf::RenderWindow& window) {
    for (const auto& arrow : activeArrows) {
        arrow.draw(window);
    }
}
