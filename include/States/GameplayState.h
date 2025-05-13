#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include <memory>
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
};

#endif // GAMEPLAYSTATE_H