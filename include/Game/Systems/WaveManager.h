#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "../include/DataStructures/DynamicArray.h"
#include "../include/Game/Enemies/Enemy.h"
#include "../include/Game/Grid/Grid.h"
#include "../include/Game/Genetics/Chromosome.h"
#include <memory>

struct EnemyPerformance {
    bool reachedEnd;
    float distanceTraveled;
    float damageReceived;
    float timeAlive;
    float damageEffectiveness;
    int chromosomeIndex;

    EnemyPerformance()
        : reachedEnd(false), distanceTraveled(0.0f), damageReceived(0.0f),
          timeAlive(0.0f), damageEffectiveness(0.0f), chromosomeIndex(-1) {}
};

class WaveManager {
private:
    int currentWave;
    DynamicArray<sf::Vector2f> enemyPath;
    Grid* gridReference;
    sf::Vector2f goalPoint;
    sf::Vector2f spawnPosition;
    float enemySpawnInterval;
    bool waveInProgress;
    int enemiesSpawned;
    int enemiesRemaining;
    int enemiesPerWave;
    sf::Clock enemySpawnTimer;
    DynamicArray<Chromosome> currentWaveChromosomes;
    std::unordered_map<int, EnemyPerformance> enemyPerformanceData;
    float pathTotalLength;
    float lastWaveMaxProgress;
    float lastWaveAvgProgress;
    float lastWaveAvgDamage;
    float lastWaveEffectiveness;
    int lastWaveReachedCount;

public:
    WaveManager(const DynamicArray<sf::Vector2f>& path, Grid* grid, const sf::Vector2f& goal, float enemySpawnInterval = 1.5f);
    DynamicArray<std::unique_ptr<Enemy>> update(float dt);
    void startNextWave();
    void setWaveChromosomes(const DynamicArray<Chromosome>& chromosomes);
    void trackEnemyPerformance(int enemyId, bool reachedEnd, float distanceTraveled, float damageReceived, float timeAlive);
    void trackEnemyDeath(int enemyId, const Enemy& enemy);
    void trackEnemyReachedEnd(int enemyId, const Enemy& enemy);
    int getCurrentWave() const;
    int getEnemiesRemaining() const;
    int getEnemiesSpawned() const { return enemiesSpawned; }
    int getEnemiesPerWave() const { return enemiesPerWave; }
    bool isWaveInProgress() const;
    bool isWaveComplete() const;
    DynamicArray<bool> getEnemiesReachedEnd() const;
    DynamicArray<float> getDistancesTraveled() const;
    DynamicArray<float> getDamagesReceived() const;
    DynamicArray<float> getTimesAlive() const;
    DynamicArray<float> getDamageEffectiveness() const;
    float getLastWaveAverageDamage() const { return lastWaveAvgDamage; }
    float getLastWaveEffectiveness() const { return lastWaveEffectiveness; }
    int getLastWaveReachedCount() const { return lastWaveReachedCount; }
    float getWaveDifficulty() const;
    void setPathTotalLength(float length);
    void reset();

private:
    void calculatePathLength();
    void calculateNextWaveEnemyCount();
    void analyzeWavePerformance();
    void adjustDifficultyBasedOnPerformance();
    float calculateAverageMetric(const DynamicArray<float>& values) const;
    float calculateMaxMetric(const DynamicArray<float>& values) const;
};