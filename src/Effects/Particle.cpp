#include "../../include/Effects/Particle.h"



// constructor
Particle::Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color, float lifetime, float size)
    : position(position), velocity(velocity), color(color), lifetime(lifetime), maxLifetime(lifetime), size(size) {
}



// actualiza la posicion segun la velocidad y el tiempo
void Particle::update(float dt) {
    position += velocity * dt;
    lifetime -= dt;
}



// getter para obtener el porcentaje de vida restante de la particula
float Particle::getLifePercentage() const {
    return lifetime / maxLifetime;
}