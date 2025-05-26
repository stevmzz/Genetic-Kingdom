#pragma once

#include <vector>
#include <memory>
#include <map>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include "../Enemies/Enemy.h"
#include "../Genetics/Chromosome.h"
#include "../include/DataStructures/DynamicArray.h"
#include "../Grid/Grid.h"
#include "../Systems/Pathfinding.h"

class WaveManager {
private:
    int currentWave;
    DynamicArray<sf::Vector2f> enemyPath;
    sf::Vector2f spawnPosition;
    float enemySpawnInterval;
    bool waveInProgress;
    int enemiesSpawned;
    int enemiesRemaining;
    int enemiesPerWave;
    DynamicArray<Chromosome> currentWaveChromosomes;

    struct EnemyPerformance {
        bool reachedEnd;
        float distanceTraveled;
        float timeAlive;
        int chromosomeIndex;
    };

    std::map<int, EnemyPerformance> enemyPerformanceData;
    float pathTotalLength;
    float lastWaveMaxProgress;
    float lastWaveAvgProgress;
    sf::Clock enemySpawnTimer;
    Grid* gridReference;
    sf::Vector2f goalPoint;

public:
    WaveManager(const DynamicArray<sf::Vector2f>& path, Grid* grid, const sf::Vector2f& goal, float enemySpawnInterval = 2.0f);

    DynamicArray<std::unique_ptr<Enemy>> update(float dt);
    int getCurrentWave() const;
    int getEnemiesRemaining() const;
    bool isWaveInProgress() const;
    void startNextWave();
    void reset();
    void setWaveChromosomes(const DynamicArray<Chromosome>& chromosomes);
    void trackEnemyPerformance(int enemyId, bool reachedEnd, float distanceTraveled, float timeAlive);
    DynamicArray<bool> getEnemiesReachedEnd() const;
    DynamicArray<float> getDistancesTraveled() const;
    DynamicArray<float> getTimesAlive() const;
    bool isWaveComplete() const;
    int getEnemiesSpawned() const { return enemiesSpawned; }
    int getEnemiesPerWave() const { return enemiesPerWave; }
    void calculateNextWaveEnemyCount();
    void setPathTotalLength(float length);
    float getMaxProgressLastWave() const { return lastWaveMaxProgress; }
    float getAvgProgressLastWave() const { return lastWaveAvgProgress; }
    void calculatePathLength();
};