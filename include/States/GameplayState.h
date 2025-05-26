#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include <memory>
#include <UI/Button.h>

#include "../include/Game/Grid/Grid.h"
#include "../include/Game/Enemies/Enemy.h"
#include "../include/Game/Systems/WaveManager.h"
#include "../include/Game/Genetics/Genetics.h"
#include "../DataStructures/DynamicArray.h"

class GameplayState : public GameState {
private:
    sf::Text gameplayText;
    std::unique_ptr<Grid> gameGrid;
    const int GRID_ROWS = 11;
    const int GRID_COLS = 20;
    const float CELL_SIZE = 70.0f;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Vector2f spawnPoint;
    sf::Vector2f goalPoint;
    DynamicArray<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<WaveManager> waveManager;
    std::unique_ptr<Genetics> geneticsSystem;
    int enemiesKilled;
    bool gameOver;
    DynamicArray<std::shared_ptr<Button>> towerButtons;
    DynamicArray<std::shared_ptr<sf::Text>> towerPriceTexts;
    Cell* selectedCellForPlacement = nullptr;
    bool clickedOutsideButtonsAndSelectedCell(const sf::Vector2f& mousePos) const;
    int playerGold = 200; // oro inicial
    sf::Text insufficientGoldText;
    sf::Clock goldWarningClock;
    bool showGoldWarning = false;
    sf::Text upgradeGoldText;
    bool showUpgradeGoldText = false;
    bool backgroundLoaded;
    sf::RectangleShape greenBackground;
    bool musicPaused;
    sf::Text pathBlockedText;
    sf::Clock pathBlockedClock;
    bool showPathBlocked = false;

public:
    GameplayState();
    ~GameplayState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
    void initializeSpawnAndGoalPoints();
    void prepareNextGeneration();
    void handleTowerAttacks(float dt);
    bool canPlaceTowerAt(Cell* cell);
    void recalculateEnemyPaths();
    bool loadBackgroundTexture();
    void startGameplayMusic();
    void pauseMusic();
    void resumeMusic();
    void stopMusic();
    void loadGameplaySounds();

    enum class TowerType {
        Archer,
        Mage,
        Gunner
    };

    TowerType selectedTowerType;
};

#endif // GAMEPLAYSTATE_H