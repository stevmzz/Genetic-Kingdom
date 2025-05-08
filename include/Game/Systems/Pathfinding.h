#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>

class Pathfinding {
public:
    static sf::Vector2f moveAlongPath(
        const sf::Vector2f& currentPosition,
        const std::vector<sf::Vector2f>& path,
        size_t& currentPathIndex,
        float speed,
        float dt,
        float arrivalThreshold = 5.0f
    );

    static bool hasReachedEnd(size_t currentPathIndex, const std::vector<sf::Vector2f>& path);
    static sf::Vector2f getDirection(const sf::Vector2f& from, const sf::Vector2f& to);
    static float getDistance(const sf::Vector2f& from, const sf::Vector2f& to);
};