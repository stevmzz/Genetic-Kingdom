#pragma once

#include <SFML/Graphics.hpp>
#include "../include/DataStructures/DynamicArray.h"
#include "../include/Game/Genetics/Chromosome.h"

struct FloatingDamageText {
    sf::Text text;
    sf::Clock timer;

    FloatingDamageText() {
        timer.restart();
    }
};

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
    float totalDamageReceived;
    sf::Clock lifeTimer;
    DynamicArray<FloatingDamageText> floatingTexts;
    static sf::Font sharedFont;

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

    virtual ~Enemy() = default;
    virtual void update(float dt);
    virtual void takeDamage(float amount, const std::string& damageType);
    bool loadTexture(const std::string& filename);
    bool isAlive() const;
    bool hasReachedEnd() const;
    void setPath(const DynamicArray<sf::Vector2f>& newPath);
    void recalculatePath(class Grid* grid, const sf::Vector2f& goal);
    int getGoldReward() const;
    sf::Vector2f getPosition() const;
    int getId() const;
    void setId(int id);
    float getTotalDistanceTraveled() const;
    float getTotalDamageReceived() const;
    float getTimeAlive() const;
    float getDamageEffectiveness() const;
    virtual void receiveDamage(float damage);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    static void setSharedFont(const sf::Font& font);

protected:
    void updateFloatingTexts(float dt);
    void createDamageText(float damage);
    void updateMovement(float dt);
    void trackDamage(float damage);
};