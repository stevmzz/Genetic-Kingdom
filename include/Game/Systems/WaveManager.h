#pragma once

#include <vector>
#include <memory>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include "../Enemies/Enemy.h"

class WaveManager {
private:
    int currentWave;
    std::vector<sf::Vector2f> enemyPath;
    sf::Vector2f spawnPosition;
    sf::Clock waveTimer;
    sf::Clock enemySpawnTimer;
    int enemiesPerWave;
    float waveInterval;
    float enemySpawnInterval;
    bool waveInProgress;
    int enemiesSpawned;
    int enemiesRemaining;

public:
    WaveManager(const std::vector<sf::Vector2f>& path,
                int enemiesPerWave = 3,
                float waveInterval = 10.0f,
                float enemySpawnInterval = 1.5f);

    std::vector<std::unique_ptr<Enemy>> update(float dt);
    int getCurrentWave() const;
    int getEnemiesRemaining() const;
    bool isWaveInProgress() const;
    void startNextWave();
    void reset();
};