#ifndef PAUSESTATE_H
#define PAUSESTATE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../Core/Game.h"
#include "../UI/Button.h"
#include "GameState.h"
#include "../Effects/ParticleSystem.h"

class PauseState : public GameState {
private:
    sf::RectangleShape background;
    sf::Text pauseText;
    std::vector<std::unique_ptr<Button>> buttons;
    std::unique_ptr<ParticleSystem> particleSystem;

    int selectedButtonIndex;

public:
    PauseState();
    ~PauseState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
};

#endif // PAUSESTATE_H