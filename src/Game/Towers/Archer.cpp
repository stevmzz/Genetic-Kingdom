//
// Created by josep on 5/21/25.
//

#include "Game/Towers/Archer.h"
#include "Game/Enemies/Enemy.h"

void Archer::attack(Enemy& enemy) {
    float elapsed = attackClock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f / attackSpeed) {
        std::cout << "Archer shoots causing " << damage << " damage.\n";
        enemy.receiveDamage(damage);
        attackClock.restart(); // reinicia el temporizador
    }
    // if (specialClock.getElapsedTime().asSeconds() >= specialCooldown) {
    //     // lanzar ataque especial (si aplica)
    //     std::cout << type() << " uses special ability!\n";
    //     specialClock.restart();
    // }
}