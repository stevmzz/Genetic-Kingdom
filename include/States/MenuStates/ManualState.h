#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "./States/GameState.h"
#include "../include/UI/Button.h"

class ManualState : public GameState {
private:
  sf::RectangleShape background;
  std::vector<std::unique_ptr<Button>> buttons;

public:
  ManualState();
  ~ManualState();

  void init();
  void handleEvents(sf::Event& event);
  void update(float dt);
  void render(sf::RenderWindow& window);
  void cleanup();
};