#pragma once
#include <SFML/Graphics.hpp>

class Cannonball {
public:
    Cannonball(const sf::Texture& texture, const sf::Vector2f& start, const sf::Vector2f& target, float speed = 400.f);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    bool hasReachedTarget() const;
    void setScale(float scale);

private:
    sf::Sprite sprite;
    sf::Vector2f direction;
    sf::Vector2f targetPos;
    float speed;
    bool reachedTarget = false;
};
