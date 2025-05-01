#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
    Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color, float lifetime, float size);
    void update(float dt);
    float getLifePercentage() const;
};

#endif // PARTICLE_H