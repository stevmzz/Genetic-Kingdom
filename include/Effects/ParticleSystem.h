#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Particle.h"

class ParticleSystem : public sf::Drawable {
private:
    std::vector<Particle> particles;
    sf::VertexArray vertices;
    sf::Vector2f emitterPosition;
    sf::Vector2f emitterSize;
    sf::Color startColor;
    sf::Color endColor;
    float particleLifetime;
    float emissionRate;
    float timeSinceLastEmit;
    float minSize;
    float maxSize;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    ParticleSystem(sf::Vector2f emitterPosition, sf::Vector2f emitterSize,
                 sf::Color startColor, sf::Color endColor,
                 float particleLifetime, float emissionRate,
                 float minSize, float maxSize);

    void update(float dt);
    void setEmitterPosition(sf::Vector2f position);
    void setEmitterSize(sf::Vector2f size);
};

#endif // PARTICLESYSTEM_H