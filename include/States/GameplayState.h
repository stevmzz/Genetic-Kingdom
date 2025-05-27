#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include <memory>
#include "../include/Game/Grid/Grid.h"
#include "../include/Game/Systems/WaveManager.h"
#include "../include/Game/Genetics/Genetics.h"
#include "../include/Game/Enemies/Enemy.h"
#include "../include/UI/Button.h"
#include "../include/UI/StatsPanel.h"

enum class TowerType {
    Archer,
    Mage,
    Gunner
};

class GameplayState : public GameState {
private:
    std::unique_ptr<Grid> gameGrid;
    const int GRID_ROWS = 11;
    const int GRID_COLS = 20;
    const float CELL_SIZE = 70.0f;
    DynamicArray<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<WaveManager> waveManager;
    std::unique_ptr<Genetics> geneticsSystem;
    sf::Vector2f spawnPoint;
    sf::Vector2f goalPoint;
    int playerGold = 150;
    int enemiesKilled;
    bool gameOver;
    bool musicPaused;
    TowerType selectedTowerType;
    class Cell* selectedCellForPlacement = nullptr;
    DynamicArray<std::shared_ptr<Button>> towerButtons;
    DynamicArray<std::shared_ptr<sf::Text>> towerPriceTexts;
    sf::Text insufficientGoldText;
    sf::Text pathBlockedText;
    sf::Text upgradeGoldText;
    bool showGoldWarning = false;
    bool showPathBlocked = false;
    bool showUpgradeGoldText = false;
    sf::Clock goldWarningClock;
    sf::Clock pathBlockedClock;
    sf::RectangleShape greenBackground;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool backgroundLoaded = false;
    std::unique_ptr<StatsPanel> statsPanel;
    int enemiesKilledThisWave = 0;
    DynamicArray<float> currentWaveFitnessList;
    int currentWaveForStats = 0;

public:
    GameplayState();
    ~GameplayState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
    void pauseMusic();
    void resumeMusic();

private:
    void loadGameplaySounds();
    void startGameplayMusic();
    bool loadBackgroundTexture();
    void initializeSpawnAndGoalPoints();
    void stopMusic();
    bool canPlaceTowerAt(class Cell* cell);
    void recalculateEnemyPaths();
    void handleTowerAttacks(float dt);
    void prepareNextGeneration();
    void updateStatsPanel();
    void collectEnemyPerformanceData();
    bool clickedOutsideButtonsAndSelectedCell(const sf::Vector2f& mousePos) const;
    void processEnemyDeath(std::unique_ptr<Enemy>& enemy);
    void processEnemyReachedEnd(std::unique_ptr<Enemy>& enemy);
    void updateEnemyStates(float dt);
};