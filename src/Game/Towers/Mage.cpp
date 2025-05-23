//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Mage.h"
#include "Game/Enemies/Enemy.h"

void Mage::attack(Enemy& enemy) {
    float elapsed = attackClock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f / attackSpeed) {
        std::cout << "Mage shoots causing " << damage << " damage.\n";
        enemy.receiveDamage(damage);
        attackClock.restart(); // reinicia el temporizador
    }
    // if (specialClock.getElapsedTime().asSeconds() >= specialCooldown) {
    //     // lanzar ataque especial (si aplica)
    //     std::cout << type() << " uses special ability!\n";
    //     specialClock.restart();
    // }
}