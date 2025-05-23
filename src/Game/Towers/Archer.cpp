//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Archer.h"
#include "Game/Enemies/Enemy.h"


Archer::Archer() : Tower(
    50, // cost
    30, // damage
    300.0f, // range
    1.5f, // attack speed
    5.0f // special cooldown
    ) {
    texture.loadFromFile("assets/images/towers/Archer.png");
    sprite.setTexture(texture);

    // Centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Escalar el sprite (ajusta el tamaño a tu celda)
    float scaleFactor = 40.f / bounds.width; // Asumiendo que la celda mide 40x40 px
    sprite.setScale(scaleFactor, scaleFactor);

}

void Archer::attack(Enemy& enemy, const std::vector<std::unique_ptr<Enemy>>&) {
    // ataque normal
    float elapsed = attackClock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f / attackSpeed) {
        enemy.receiveDamage(damage);
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
            enemy.receiveDamage(damage*0.8);
            burstShotsFired++;
            burstClock.restart();
        }
    }

    // terminar la ráfaga
    if (burstActive && burstShotsFired >= totalBurstShots) {
        burstActive = false;
    }
}