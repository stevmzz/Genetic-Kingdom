#include "../../include/Effects/ParticleSystem.h"
#include <random>
#include <cmath>



// constructor del sistema de particulas, inicializa la posicion del emisor, color inicial y final, tiempo de vida, tasa de emision y tamaños
ParticleSystem::ParticleSystem(sf::Vector2f emitterPosition, sf::Vector2f emitterSize,
                             sf::Color startColor, sf::Color endColor,
                             float particleLifetime, float emissionRate,
                             float minSize, float maxSize)
    : emitterPosition(emitterPosition), emitterSize(emitterSize),
      startColor(startColor), endColor(endColor),
      particleLifetime(particleLifetime), emissionRate(emissionRate),
      timeSinceLastEmit(0), minSize(minSize), maxSize(maxSize) {

    // se usa sf::Points para dibujar particulas individuales como puntos
    vertices.setPrimitiveType(sf::Points);
}



// actualiza el sistema de particulas en cada frame
void ParticleSystem::update(float dt) {
    // actualizar particulas existentes
    for (size_t i = 0; i < particles.size(); ) {
        particles[i].update(dt);

        // eliminar particulas que ya expiraron
        if (particles[i].lifetime <= 0) {
            particles.erase(particles.begin() + i);
        } else {
            i++;
        }
    }

    // emitir nuevas particulas segun la tasa de emision
    timeSinceLastEmit += dt;
    float emissionPeriod = 1.0f / emissionRate;

    while (timeSinceLastEmit >= emissionPeriod) {
        // generadores aleatorios para posicion, velocidad, vida y tamaño
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> posX(emitterPosition.x, emitterPosition.x + emitterSize.x);
        std::uniform_real_distribution<float> posY(emitterPosition.y, emitterPosition.y + emitterSize.y);
        std::uniform_real_distribution<float> velX(-5.0f, 5.0f);
        std::uniform_real_distribution<float> velY(-10.0f, -5.0f);
        std::uniform_real_distribution<float> lifeTime(particleLifetime * 0.8f, particleLifetime * 1.2f);
        std::uniform_real_distribution<float> size(minSize, maxSize);

        // crear particula con propiedades aleatorias
        sf::Vector2f position(posX(gen), posY(gen));
        sf::Vector2f velocity(velX(gen), velY(gen));

        particles.emplace_back(position, velocity, startColor, lifeTime(gen), size(gen));

        // reducir el tiempo acumulado para la siguiente emision
        timeSinceLastEmit -= emissionPeriod;
    }

    // actualizar vertices para representar visualmente las particulas
    vertices.clear();
    for (const auto& particle : particles) {
        float alpha = particle.getLifePercentage();
        sf::Color currentColor = sf::Color(
            startColor.r,
            startColor.g,
            startColor.b,
            static_cast<sf::Uint8>(startColor.a * alpha) // transparencia segun vida restante
        );

        sf::Vertex vertex(particle.position, currentColor);
        vertices.append(vertex);
    }
}



// dibuja las particulas en el target dado
void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // se pueden configurar estados adicionales si se usan shaders
    sf::RenderStates pointStates = states;

    // dibujar particulas como puntos
    target.draw(vertices, pointStates);

    // dibujar particulas como circulos para darles tamaño visual
    for (const auto& particle : particles) {
        float alpha = particle.getLifePercentage();
        sf::Color currentColor = sf::Color(
            startColor.r,
            startColor.g,
            startColor.b,
            static_cast<sf::Uint8>(startColor.a * alpha)
        );

        sf::CircleShape circle(particle.size);
        circle.setPosition(particle.position - sf::Vector2f(particle.size, particle.size));
        circle.setFillColor(currentColor);
        target.draw(circle, states);
    }
}



// actualiza la posicion del emisor
void ParticleSystem::setEmitterPosition(sf::Vector2f position) {
    emitterPosition = position;
}



// actualiza el tamaño del area del emisor
void ParticleSystem::setEmitterSize(sf::Vector2f size) {
    emitterSize = size;
}
