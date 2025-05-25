#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "../Genetics/Chromosome.h"
#include "./DataStructures/DynamicArray.h"

class Grid;

class Enemy : public sf::Drawable {
protected:
    int id;
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
    DynamicArray<sf::Vector2f> path;
    size_t currentPathIndex;
    float totalDistanceTraveled;
    sf::Clock lifeTimer;

public:
    Enemy(
        float health,
        float speed,
        float arrowRes,
        float magicRes,
        float artilleryRes,
        int goldReward,
        const sf::Vector2f& position,
        const DynamicArray<sf::Vector2f>& path);

    Enemy(
        const Chromosome& chromosome,
        int goldReward,
        const sf::Vector2f& position,
        const DynamicArray<sf::Vector2f>& path);

    struct FloatingDamageText {
        sf::Text text;
        sf::Clock timer;
    };

    std::vector<FloatingDamageText> floatingTexts;

    virtual ~Enemy() = default;

    virtual void update(float dt) = 0;
    virtual void takeDamage(float amount, const std::string& damageType) = 0;

    bool isAlive() const;
    bool hasReachedEnd() const;
    int getGoldReward() const;
    sf::Vector2f getPosition() const;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void setPath(const DynamicArray<sf::Vector2f>& newPath);
    bool loadTexture(const std::string& filename);
    void setId(int id);
    int getId() const;
    float getTotalDistanceTraveled() const;
    float getTimeAlive() const;
    void receiveDamage(float damage);
    inline static sf::Font sharedFont;
    static void setSharedFont(const sf::Font& font);
    void recalculatePath(Grid* grid, const sf::Vector2f& goal);
};