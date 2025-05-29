#include "Game/Towers/Tower.h"

Tower::Tower(int cost, int damage, float range, float attackSpeed, float specialCooldown)
    : cost(cost), damage(damage), range(range), attackSpeed(attackSpeed), specialCooldown(specialCooldown) {}

float Tower::getRange() const {
    return range;
}

int Tower::getCost() const {
    return cost;
}

bool Tower::canUpgrade() const {
    return level < maxLevel;
}

int Tower::getLevel() const {
    return level;
}

void Tower::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

const sf::Sprite& Tower::getSprite() const {
    return sprite;
}

void Tower::setSharedFont(const sf::Font& font) {
    sharedFont = font;
}

const sf::Font& Tower::getFont() const {
    return sharedFont;
}

void Tower::setAudioSystem(AudioSystem* audio) {
    audioSystem = audio;
}
