#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include <memory>
#include "../include/Game/Grid/Grid.h"

class GameplayState : public GameState {
private:
    sf::Text gameplayText;
    std::unique_ptr<Grid> gameGrid;
    const int GRID_ROWS = 10;
    const int GRID_COLS = 20;
    const float CELL_SIZE = 70.0f;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

public:
    GameplayState();
    ~GameplayState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
};

#endif // GAMEPLAYSTATE_H