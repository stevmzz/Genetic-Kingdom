#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>

class Game;

class GameState {
public:
    virtual ~GameState() {}
    virtual void init() = 0;
    virtual void handleEvents(sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void cleanup() = 0;
    void setGame(Game* game) { this->game = game; }

protected:
    Game* game;
};

#endif // GAMESTATE_H