#include "../include/States/MenuStates/CreditsState.h"
#include "../include/Core/Game.h"
#include "../include/States/MenuStates/MenuState.h"
#include <iostream>

// constructor del estado de creditos
CreditsState::CreditsState() {}



// destructor del estado de creditos
CreditsState::~CreditsState() {}



// inicializa el estado de creditos
void CreditsState::init() {
    auto& window = game->getWindow();
    auto& font = game->getFont();



    // fondo
    background.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color(25, 25, 25));

    float buttonWidth = 200.f;
    float buttonHeight = 50.f;
    float buttonX = (window.getSize().x - buttonWidth) / 2;
    float buttonY = window.getSize().y * 0.8f;

    auto exitButton = std::make_unique<Button>(
        buttonX, buttonY,
        buttonWidth, buttonHeight,
        font, "EXIT",
        [this]() {
            auto menuState = std::make_shared<MenuState>();
            game->changeState(menuState);
        }
    );

    exitButton->setTextures(
        "assets/images/buttons/boton_deselected.png",
        "assets/images/buttons/boton_selected.png"
    );

    buttons.push_back(std::move(exitButton));
    if (!buttons.empty()) {
        buttons[0]->setSelected(true);
    }
}



// maneja los eventos del estado de creditos
void CreditsState::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Return:
                if (!buttons.empty()) {
                    buttons[0]->activate();
                }
            break;

            case sf::Keyboard::Escape:
                game->quit();
            break;
        }
    }
}



// dibuja el contenido del estado de creditos en la ventana
void CreditsState::render(sf::RenderWindow& window) {
    window.draw(background);

    for (auto& button : buttons) {
        button->draw(window);
    }
}



// actualiza la logica del estado de creditos
void CreditsState::update(float dt) {}



// libera recursos del estado de creditos
void CreditsState::cleanup() {
    buttons.clear();
}