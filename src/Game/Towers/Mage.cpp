//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Mage.h"
#include "Game/Enemies/Enemy.h"
#include "Core/AudioSystem.h"

Mage::Mage() : Tower(
    150, // cost
    45, // damage
    200.0f, // range
    1.2f, // attack speed
    7.0f // special cooldown
    ) {
    texture.loadFromFile("assets/images/towers/Mage.png");
    sprite.setTexture(texture);

    fireballTexture.loadFromFile("assets/images/towers/Fireball.png");

    // Centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Escalar el sprite (ajusta el tamaño a tu celda)
    float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
    sprite.setScale(scaleFactor, scaleFactor);
}

void Mage::attack(Enemy& enemy, const DynamicArray<std::unique_ptr<Enemy>>& allEnemies) {
    float elapsed = attackClock.getElapsedTime().asSeconds();

    // ataque normal
    if (elapsed >= 1.0f / attackSpeed) {
        enemy.takeDamage(damage, "magic");

        //  sonido de fireball
        if (audioSystem) {
            audioSystem->playSound("fireball");
        }

        // Animacion de bola de fuego
        Fireball fb(fireballTexture, sprite.getPosition(), enemy.getPosition());
        activeFireballs.push_back(fb);

        attackClock.restart();
    }

    // ataque especial
    // el ataque especial hace dano en area
    float specialElapsed = specialClock.getElapsedTime().asSeconds();
    if (specialElapsed >= specialCooldown) {
        float roll = static_cast<float>(rand()) / RAND_MAX;
        if (roll <= specialChance) {
            std::cout << "Mage fires an area explosion\n";
            float aoeRadius = 200.0f;
            float extraDamage = damage * 1.5f;

            //  sonido de fireball
            if (audioSystem) {
                audioSystem->playSound("fireball");
            }

            // Crear animación especial de bola de fuego grande
            Fireball specialFb(fireballTexture, sprite.getPosition(), enemy.getPosition());
            specialFb.setScale(0.2f); // más grande
            activeFireballs.push_back(specialFb);

            // daño fuerte al objetivo principal
            enemy.takeDamage(extraDamage, "magic");

            // daño en área a enemigos cercanos
            for (const auto& other : allEnemies) {
                if (other.get() == &enemy || !other->isAlive()) continue;

                float dx = other->getPosition().x - enemy.getPosition().x;
                float dy = other->getPosition().y - enemy.getPosition().y;
                float dist = std::hypot(dx, dy);

                if (dist <= aoeRadius) {
                    std::cout << "Explosion hits nearby enemy for " << damage << " damage.\n";
                    other->takeDamage(damage*1.2, "magic");
                }
            }
        }
        specialClock.restart();
    }
}

int Mage::getUpgradeCost() const {
    return 80 + (level * 60); // Nivel 1→150, 2→225
}

void Mage::upgrade() {
    if (canUpgrade()) {
        level++;
        damage += 18;            // mejora moderada
        range += 20.0f;          // rango estable
        attackSpeed += 0.12f;    // mejora ligera en velocidad
        specialCooldown -= 0.2f; // ataque especial más frecuente

        recentlyUpgraded = true;
        upgradeFlashClock.restart();

        std::cout << "Mage upgraded to level " << level << "\n";
    }
}

void Mage::updateProjectiles(float dt) {
    for (auto it = activeFireballs.begin(); it != activeFireballs.end(); ) {
        it->update(dt);
        if (it->hasReachedTarget()) {
            it = activeFireballs.erase(it); // eliminar fireballs que ya impactaron
        } else {
            ++it;
        }
    }
}

void Mage::drawProjectiles(sf::RenderWindow& window) {
    for (const auto& fireball : activeFireballs) {
        fireball.draw(window);
    }
}