#include "../include/States/PauseState.h"
#include "../include/States/MenuStates/MenuState.h"
#include "../include/Core/Game.h"



// constructor del estado de pausa
PauseState::PauseState() : selectedButtonIndex(0) {
}



// destructor del estado de pausa
PauseState::~PauseState() {
}



// inicializa el estado de pausa
void PauseState::init() {
    auto& window = game->getWindow(); // obtiene la referencia a la ventana
    auto& font = game->getFont(); // obtiene la fuente del juego

    // poner cursos invisible
    game->getWindow().setMouseCursorVisible(false);

    // fondo semitransparente
    background.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color(0, 0, 0, 50));

    // texto de pausa
    pauseText.setFont(font);
    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(36);
    pauseText.setFillColor(sf::Color::White);

    // centrar el texto
    sf::FloatRect textRect = pauseText.getLocalBounds();
    pauseText.setPosition(
        (window.getSize().x - textRect.width) / 2,
        window.getSize().y * 0.3f
    );

    ///// PARTICULAS /////
    // crear el sistema de particulas
    sf::Color particleStartColor(150, 50, 50, 100);
    sf::Color particleEndColor(150, 50, 50, 0);
    particleSystem = std::make_unique<ParticleSystem>(
        sf::Vector2f(0, 0),
        sf::Vector2f(window.getSize()),
        particleStartColor,
        particleEndColor,
        5.0f,
        50.0f,
        1.0f,
        3.0f
    );

    // boton "return to menu"
    float buttonWidth = 350.f;
    float buttonHeight = 100.f;
    float buttonX = (window.getSize().x - buttonWidth) / 2;
    float buttonY = window.getSize().y * 0.5f;

    buttons.push_back(std::make_unique<Button>(
        buttonX, buttonY,
        buttonWidth, buttonHeight,
        font, "Return to Menu",
        [this]() {
            auto menuState = std::make_shared<MenuState>();
            game->changeState(menuState);
        }
    ));
    buttons[0]->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
);

    // boton "continue"
    buttons.push_back(std::make_unique<Button>(
        buttonX, buttonY + buttonHeight + 20,
        buttonWidth, buttonHeight,
        font, "Continue",
        [this]() {
            game->popState(); // vuelve al estado anterior
            game->getWindow().setMouseCursorVisible(true);
        }
    ));
    buttons[1]->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
);

    // seleccionar el primer boton por defecto
    if (!buttons.empty()) {
        buttons[selectedButtonIndex]->setSelected(true);
    }
}



// maneja los eventos del estado de pausa
void PauseState::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {

            // mover hacia arriba
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if (!buttons.empty()) {
                    buttons[selectedButtonIndex]->setSelected(false);
                    selectedButtonIndex = (selectedButtonIndex - 1 + buttons.size()) % buttons.size();
                    buttons[selectedButtonIndex]->setSelected(true);
                }
                break;

            // mover hacia abajo
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (!buttons.empty()) {
                    buttons[selectedButtonIndex]->setSelected(false);
                    selectedButtonIndex = (selectedButtonIndex + 1) % buttons.size();
                    buttons[selectedButtonIndex]->setSelected(true);
                }
                break;

            // enter
            case sf::Keyboard::Return:
                if (!buttons.empty()) {
                    buttons[selectedButtonIndex]->activate();
                }
                break;

            // escape
            case sf::Keyboard::Escape:
                game->popState(); // vuelve al juego
                game->getWindow().setMouseCursorVisible(true);
                break;
        }
    }
}



// actualiza la logica del estado de pausa
void PauseState::update(float dt) {
    // actualizar sistema de particulas
    if (particleSystem) {
        particleSystem->update(dt);
    }
}



// dibuja el contenido del estado de pausa
void PauseState::render(sf::RenderWindow& window) {
    window.draw(background);

    if (particleSystem) {
        window.draw(*particleSystem);
    }

    window.draw(pauseText);

    for (auto& button : buttons) {
        button->draw(window);
    }
}



// libera los recursos del estado de pausa
void PauseState::cleanup() {
    buttons.clear();
    particleSystem.reset();
}
