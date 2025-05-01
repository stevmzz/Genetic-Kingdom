#pragma once

#include <SFML/Graphics.hpp>
#include <stack>
#include <memory>
#include "AudioSystem.h"

class GameState;

class Game {
private:
    bool running;
    sf::RenderWindow window;
    sf::Font font;
    sf::Clock clock;
    std::stack<std::shared_ptr<GameState>> states;
    AudioSystem audioSystem;

public:
    Game();
    ~Game();

    void init();
    void run();
    void quit() { running = false; }
    void pushState(std::shared_ptr<GameState> state);
    void popState();
    void changeState(std::shared_ptr<GameState> state);
    std::shared_ptr<GameState> getCurrentState();
    sf::RenderWindow& getWindow() { return window; }
    sf::Font& getFont() { return font; }
    AudioSystem& getAudioSystem() { return audioSystem; }
    float getMasterVolume() const { return audioSystem.getMasterVolume(); }
    void setMasterVolume(float volume) { audioSystem.setMasterVolume(volume); }
    float getMusicVolume() const { return audioSystem.getMusicVolume(); }
    void setMusicVolume(float volume) { audioSystem.setMusicVolume(volume); }
    float getSfxVolume() const { return audioSystem.getSfxVolume(); }
    void setSfxVolume(float volume) { audioSystem.setSfxVolume(volume); }
};