#include "../include/Game/Systems/Pathfinding.h"
#include <cmath>

// metodo para movimiento lineal basico entre puntos
sf::Vector2f Pathfinding::moveAlongPath(
    const sf::Vector2f& currentPosition,
    const std::vector<sf::Vector2f>& path,
    size_t& currentPathIndex,
    float speed,
    float dt,
    float arrivalThreshold) {

    // verificar si el camino es valido y si no hemos llegado al final
    if (path.empty() || currentPathIndex >= path.size()) {
        return currentPosition;
    }

    // obtener el punto objetivo actual
    sf::Vector2f targetPoint = path[currentPathIndex];

    // calcular direccion y distancia
    sf::Vector2f moveDirection = getDirection(currentPosition, targetPoint);
    float distance = getDistance(currentPosition, targetPoint);

    // si estamos cerca del punto objetivo, avanzar al siguiente punto
    if (distance < arrivalThreshold) {
        currentPathIndex++;

        // si llegamos al final, devolver la posicion actual
        if (currentPathIndex >= path.size()) {
            return currentPosition;
        }

        // obtener el nuevo objetivo
        targetPoint = path[currentPathIndex];
        moveDirection = getDirection(currentPosition, targetPoint);
    }

    // calcular la nueva posicion
    return currentPosition + moveDirection * speed * dt;
}



// verifica si ha llegado al final del camino
bool Pathfinding::hasReachedEnd(size_t currentPathIndex, const std::vector<sf::Vector2f>& path) {
    return currentPathIndex >= path.size() - 1;
}



// calcula la direccion normalizada entre dos puntos
sf::Vector2f Pathfinding::getDirection(const sf::Vector2f& from, const sf::Vector2f& to) {
    sf::Vector2f direction = to - from;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // evitar division por cero
    if (length > 0.0001f) {
        direction /= length;
    }

    return direction;
}



// calcula la distancia entre dos puntos
float Pathfinding::getDistance(const sf::Vector2f& from, const sf::Vector2f& to) {
    sf::Vector2f delta = to - from;
    return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}