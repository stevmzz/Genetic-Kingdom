#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include "./Core/Game.h"
#include "./UI/Button.h"
#include "./Effects/ParticleSystem.h"
#include "./States/GameState.h"

class MenuState : public GameState {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture logoTexture;
    sf::Sprite logoSprite;
    std::unique_ptr<ParticleSystem> particleSystem;
    std::vector<std::unique_ptr<Button>> buttons;

    int selectedButtonIndex;
    sf::Clock buttonActivationClock;
    int pendingButtonActivation;

public:
    MenuState();
    ~MenuState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
    void setMusicVolume(float volume);
};

#endif // MENUSTATE_H