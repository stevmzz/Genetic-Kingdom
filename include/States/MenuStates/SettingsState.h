#pragma once

#include "../GameState.h"
#include "../../UI/Button.h"
#include "../../UI/Slider.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class SettingsState : public GameState {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::vector<std::unique_ptr<Button>> buttons;
    std::unique_ptr<Slider> masterVolumeSlider;
    std::unique_ptr<Slider> musicVolumeSlider;
    std::unique_ptr<Slider> sfxVolumeSlider;
    int selectedControlIndex;
    sf::Text settingsTitle;
    sf::Text masterVolumeLabel;
    sf::Text musicVolumeLabel;
    sf::Text sfxVolumeLabel;

public:
    SettingsState();
    ~SettingsState();

    void init() override;
    void handleEvents(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void cleanup() override;
    void selectControl(int index);
};