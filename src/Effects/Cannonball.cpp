#include "../../include/Effects/Cannonball.h"
#include <cmath>

Cannonball::Cannonball(const sf::Texture& texture, const sf::Vector2f& start, const sf::Vector2f& target, float speed)
    : targetPos(target), speed(speed) {
    sprite.setTexture(texture);
    sprite.setPosition(start);
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    sprite.setScale(0.08f, 0.08f);

    sf::Vector2f delta = target - start;
    float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    direction = (length != 0) ? delta / length : sf::Vector2f(0.f, 0.f);
}

void Cannonball::update(float dt) {
    if (reachedTarget) return;

    sprite.move(direction * speed * dt);

    sf::Vector2f pos = sprite.getPosition();
    if ((direction.x > 0 && pos.x >= targetPos.x) || (direction.x < 0 && pos.x <= targetPos.x) ||
        (direction.y > 0 && pos.y >= targetPos.y) || (direction.y < 0 && pos.y <= targetPos.y)) {
        reachedTarget = true;
        }
}

void Cannonball::draw(sf::RenderWindow& window) const {
    if (!reachedTarget)
        window.draw(sprite);
}

bool Cannonball::hasReachedTarget() const {
    return reachedTarget;
}

void Cannonball::setScale(float scale) {
    sprite.setScale(scale, scale);
}
