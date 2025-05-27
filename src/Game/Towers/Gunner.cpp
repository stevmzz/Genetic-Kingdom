//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Gunner.h"

#include "Effects/Cannonball.h"
#include "Game/Enemies/Enemy.h"
#include "Core/AudioSystem.h"

Gunner::Gunner() : Tower(
    200,     // cost (costo alto)
    65,      // damage (daño alto)
    180.0f,  // range (alcance bajo pero no extremo)
    0.8f,    // attack speed (velocidad baja)
    8.0f     // special cooldown
    ) {
    texture.loadFromFile("assets/images/towers/Gunner.png");
    sprite.setTexture(texture);

    bulletTexture.loadFromFile("assets/images/towers/Cannonball.png");

    // Centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Escalar el sprite (ajusta el tamaño a tu celda)
    float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
    sprite.setScale(scaleFactor, scaleFactor);
}

void Gunner::attack(Enemy& enemy, const DynamicArray<std::unique_ptr<Enemy>>&) {
    float elapsed = attackClock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f / attackSpeed) {
        enemy.takeDamage(damage, "artillery");

        // sonido de bala
        if (audioSystem) {
            audioSystem->playSound("cannonball");
        }

        Cannonball bullet(bulletTexture, sprite.getPosition(), enemy.getPosition());
        bullet.setScale(0.02f); // más pequeño que fireball
        activeBullets.push_back(bullet);

        attackClock.restart(); // reinicia el temporizador
    }

    // ataque especial de alto daño
    float specialElapsed = specialClock.getElapsedTime().asSeconds();
    if (specialElapsed >= specialCooldown) {
        float roll = static_cast<float>(rand()) / RAND_MAX;
        if (roll <= specialChance) {
            float specialDamage = damage * 2.0f;
            std::cout << "Gunner fires big damage shot causing " << specialDamage << " damage\n";

            // sonido de bala
            if (audioSystem) {
                audioSystem->playSound("cannonball");
            }

            Cannonball specialBullet(bulletTexture, sprite.getPosition(), enemy.getPosition());
            specialBullet.setScale(0.06f); // más pequeño que fireball
            activeBullets.push_back(specialBullet);

            enemy.takeDamage(specialDamage, "artillery");
        }
        specialClock.restart();
    }
}

int Gunner::getUpgradeCost() const {
    return 120 + (level * 70); // Nivel 1-190, 2-260, 3-330
}

void Gunner::upgrade() {
    if (canUpgrade()) {
        level++;
        damage += 25;            // gran aumento de daño
        range += 15.0f;          // mejora leve en rango
        attackSpeed += 0.1f;     // mejora mínima en cadencia
        specialChance += 0.06f;   // aumenta probabilidad de especial

        recentlyUpgraded = true;
        upgradeFlashClock.restart();

        std::cout << "Gunner upgraded to level " << level << "\n";
    }
}

void Gunner::updateProjectiles(float dt) {
    for (auto it = activeBullets.begin(); it != activeBullets.end(); ) {
        it->update(dt);
        if (it->hasReachedTarget()) {
            it = activeBullets.erase(it);
        } else {
            ++it;
        }
    }
}

void Gunner::drawProjectiles(sf::RenderWindow& window) {
    for (const auto& bullet : activeBullets) {
        bullet.draw(window);
    }
}