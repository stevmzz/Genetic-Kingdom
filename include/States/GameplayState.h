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

class GameplayState : public GameState {
private:
    sf::Text gameplayText;
    std::unique_ptr<Grid> gameGrid;
    const int GRID_ROWS = 11;
    const int GRID_COLS = 20;
    const float CELL_SIZE = 70.0f;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::vector<sf::Vector2f> testPath;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<WaveManager> waveManager;
    std::unique_ptr<Genetics> geneticsSystem;
    int enemiesKilled;
    bool gameOver;
    std::vector<std::shared_ptr<Button>> towerButtons;
    std::vector<std::shared_ptr<sf::Text>> towerPriceTexts;
    Cell* selectedCellForPlacement = nullptr; // celda en la que se mostraron los botones
    bool clickedOutsideButtonsAndSelectedCell(const sf::Vector2f& mousePos) const;
    int playerGold = 100000; // oro inicial
    sf::Text insufficientGoldText;
    sf::Clock goldWarningClock;
    bool showGoldWarning = false;
    sf::Text upgradeGoldText;
    bool showUpgradeGoldText = false;

    //Variable para el panel de estadisticas
    std::unique_ptr<StatsPanel> statsPanel;


public:
    GameplayState();
    ~GameplayState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
    void createTestPath();
    void prepareNextGeneration();
    void handleTowerAttacks();

    enum class TowerType {
        Archer,
        Mage,
        Gunner
    };

    TowerType selectedTowerType;
};

#endif // GAMEPLAYSTATE_H