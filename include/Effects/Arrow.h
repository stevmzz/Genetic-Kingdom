#pragma once
#include <SFML/Graphics.hpp>

class Arrow {
public:
    Arrow(const sf::Texture& texture, const sf::Vector2f& start, const sf::Vector2f& target, float speed = 600.f);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    bool hasReachedTarget() const;

private:
    sf::Sprite sprite;
    sf::Vector2f direction;
    sf::Vector2f targetPos;
    float speed;
    bool reachedTarget = false;
};
