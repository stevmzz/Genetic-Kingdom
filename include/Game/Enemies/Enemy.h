#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Enemy : public sf::Drawable {
protected:
    float health;
    float maxHealth;
    float speed;
    sf::Vector2f position;
    sf::Vector2f direction;
    float arrowResistance;
    float magicResistance;
    float artilleryResistance;
    int goldReward;
    sf::Sprite sprite;
    sf::Texture texture;
    bool isActive;
    std::vector<sf::Vector2f> path;
    size_t currentPathIndex;

public:
    Enemy(float health, float speed,
          float arrowRes, float magicRes, float artilleryRes,
          int goldReward,
          const sf::Vector2f& position,
          const std::vector<sf::Vector2f>& path);
    virtual ~Enemy() = default;

    virtual void update(float dt) = 0;
    virtual void takeDamage(float amount, const std::string& damageType) = 0;

    bool isAlive() const;
    bool hasReachedEnd() const;
    int getGoldReward() const;
    sf::Vector2f getPosition() const;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void setPath(const std::vector<sf::Vector2f>& newPath);
    bool loadTexture(const std::string& filename);
};