#ifndef TOWER_H
#define TOWER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "./DataStructures/DynamicArray.h"

class Enemy; // Forward declaration
class AudioSystem; // Forward para evitar incluir directamente

class Tower {
protected:
    int cost;
    int damage;
    float range;
    float attackSpeed;
    float specialCooldown;
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Clock attackClock;
    sf::Clock specialClock;
    float specialChance = 0.8f;
    int level = 1;
    const int maxLevel = 3;

    inline static sf::Font sharedFont;
    inline static AudioSystem* audioSystem = nullptr;

public:
    Tower(int cost, int damage, float range, float attackSpeed, float specialCooldown);

    virtual ~Tower() = default;

    virtual void attack(Enemy& enemy, const DynamicArray<std::unique_ptr<Enemy>>& allEnemies) = 0;
    virtual std::string type() const = 0;

    virtual void setPosition(const sf::Vector2f& pos);
    virtual const sf::Sprite& getSprite() const;

    float getRange() const;
    int getCost() const;
    bool canUpgrade() const;
    int getLevel() const;

    virtual int getUpgradeCost() const = 0;
    virtual void upgrade() = 0;

    sf::Clock upgradeFlashClock;
    bool recentlyUpgraded = false;

    static void setSharedFont(const sf::Font& font);
    const sf::Font& getFont() const;

    static void setAudioSystem(AudioSystem* audio);
};

#endif // TOWER_H
