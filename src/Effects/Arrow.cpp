#include "Effects/Arrow.h"
#include <cmath>

Arrow::Arrow(const sf::Texture& texture, const sf::Vector2f& start, const sf::Vector2f& target, float speed)
    : targetPos(target), speed(speed) {
    sprite.setTexture(texture);
    sprite.setPosition(start);
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);

    sprite.setScale(0.08f, 0.08f); // escala al 20% del tamaño original

    sf::Vector2f delta = target - start;
    float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    if (length != 0)
        direction = delta / length;

    // Rotar el sprite para que apunte al objetivo
    float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159f;
    sprite.setRotation(angle);
}

void Arrow::update(float dt) {
    if (reachedTarget) return;

    sprite.move(direction * speed * dt);

    // Verificación simple de llegada
    sf::Vector2f pos = sprite.getPosition();
    if ((direction.x > 0 && pos.x >= targetPos.x) || (direction.x < 0 && pos.x <= targetPos.x) ||
        (direction.y > 0 && pos.y >= targetPos.y) || (direction.y < 0 && pos.y <= targetPos.y)) {
        reachedTarget = true;
        }
}

void Arrow::draw(sf::RenderWindow& window) const {
    if (!reachedTarget)
        window.draw(sprite);
}

bool Arrow::hasReachedTarget() const {
    return reachedTarget;
}
