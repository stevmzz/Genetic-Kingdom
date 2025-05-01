#include "../include/States/MenuStates/MenuState.h"
#include "../include/States/GameplayState.h"
#include "../include/Core/Game.h"
#include "../include/UI/Button.h"
#include "../include/States/MenuStates/SettingsState.h"
#include "../include/States/MenuStates/ManualState.h"
#include "../include/States/MenuStates/CreditsState.h"
#include <iostream>



// constructor del estado del menu
MenuState::MenuState() : selectedButtonIndex(0) {
}



// destructor del estado del menu
MenuState::~MenuState() {
}



// inicializa el estado del menu
void MenuState::init() {
    auto& window = game->getWindow();
    auto& font = game->getFont();



    ///// FONDO /////
    // cargar la textura de fondo
    if (!backgroundTexture.loadFromFile("assets/images/backgrounds/background.png")) {
        std::cerr << "error al cargar la imagen de fondo" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);

    // ajustar la escala para cubrir toda la ventana
    float scaleX = window.getSize().x / static_cast<float>(backgroundTexture.getSize().x);
    float scaleY = window.getSize().y / static_cast<float>(backgroundTexture.getSize().y);
    backgroundSprite.setScale(scaleX, scaleY);



    ///// LOGO /////
    // cargar el logo
    if (!logoTexture.loadFromFile("assets/images/logo.png")) {
        std::cerr << "error al cargar el logo" << std::endl;
    }
    logoSprite.setTexture(logoTexture);

    // posicionar el logo en la parte superior central
    sf::FloatRect logoBounds = logoSprite.getLocalBounds();
    logoSprite.setOrigin(logoBounds.width / 2, 0);
    logoSprite.setPosition(window.getSize().x / 2, window.getSize().y * 0.0001f);
    logoSprite.setScale(0.6f, 0.6f);



    ///// MUSICA /////
    // cargar y reproducir la musica de fondo
    if (!game->getAudioSystem().playMusicIfNotPlaying("assets/audio/menu-theme.mp3", true)) {
        std::cerr << "error al cargar la musica de fondo" << std::endl;
    }



    ///// SONIDOS /////
    // cargar sonidos de navegación y activación
    if (!game->getAudioSystem().loadSound("select", "assets/audio/select.wav")) {
        std::cerr << "error al cargar sonido" << std::endl;
    }

    if (!game->getAudioSystem().loadSound("activate", "assets/audio/activate.wav")) {
        std::cerr << "error al cargar sonido" << std::endl;
    }



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
        40.0f,
        2.0f,
        3.0f
    );



    ///// BOTONES /////
    // calcular posicion y tamanos de los botones
    float buttonWidth = 350.f;
    float buttonHeight = 80.f;
    float buttonX = (window.getSize().x - buttonWidth) / 2;
    float buttonY = window.getSize().y * 0.45f;

    // boton play
    auto playButton = std::make_unique<Button>(
        buttonX, buttonY,
        buttonWidth, buttonHeight,
        font, "New Game",
        [this]() {
            auto gameplayState = std::make_shared<GameplayState>();
            game->changeState(gameplayState);
        }
    );
    playButton->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
    );

    // boton settings
    auto settingsButton = std::make_unique<Button>(
        buttonX, buttonY + buttonHeight + 15,
        buttonWidth, buttonHeight,
        font, "Settings",
        [this]() {
            auto settingsState = std::make_shared<SettingsState>();
            game->changeState(settingsState);
        }
    );
    settingsButton->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
    );

    // boton manual
    auto manualButton = std::make_unique<Button>(
        buttonX, buttonY + buttonHeight + 110,
        buttonWidth, buttonHeight,
        font, "Manual",
        [this]() {
            auto manualState = std::make_shared<ManualState>();
            game->changeState(manualState);
        }
    );
    manualButton->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
    );

    // boton credits
    auto creditsButton = std::make_unique<Button>(
        buttonX, buttonY + buttonHeight + 205,
        buttonWidth, buttonHeight,
        font, "Credits",
        [this]() {
            auto creditsState = std::make_shared<CreditsState>();
            game->changeState(creditsState);
        }
    );
    creditsButton->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
    );

    // boton exit
    auto exitButton = std::make_unique<Button>(
        buttonX, buttonY + buttonHeight + 300,
        buttonWidth, buttonHeight,
        font, "Quit Game",
        [this]() {
            game->quit();
        }
    );
    exitButton->setTextures(
    "assets/images/boton_deselected.png",
    "assets/images/boton_selected.png"
    );

    // agregar botones a la lista
    buttons.push_back(std::move(playButton));
    buttons.push_back(std::move(settingsButton));
    buttons.push_back(std::move(manualButton));
    buttons.push_back(std::move(creditsButton));
    buttons.push_back(std::move(exitButton));

    // seleccionar el primer boton por defecto
    if (!buttons.empty()) {
        buttons[selectedButtonIndex]->setSelected(true);
    }
}



// maneja los eventos del estado del menu
void MenuState::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {

            // mover hacia arriba
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if (!buttons.empty()) {
                    buttons[selectedButtonIndex]->setSelected(false);
                    selectedButtonIndex = (selectedButtonIndex - 1 + buttons.size()) % buttons.size();
                    buttons[selectedButtonIndex]->setSelected(true);
                    game->getAudioSystem().playSound("select");
                }
            break;

            // mover hacia abajo
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (!buttons.empty()) {
                    buttons[selectedButtonIndex]->setSelected(false);
                    selectedButtonIndex = (selectedButtonIndex + 1) % buttons.size();
                    buttons[selectedButtonIndex]->setSelected(true);
                    game->getAudioSystem().playSound("select");

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
                game->quit();
            break;
        }
    }
}



// actualiza la logica del estado del menu
void MenuState::update(float dt) {
    // actualizar sistema de particulas
    if (particleSystem) {
        particleSystem->update(dt);
    }
}



// dibuja el contenido del estado del menu en la ventana
void MenuState::render(sf::RenderWindow& window) {
    // dibujar el fondo
    window.draw(backgroundSprite);

    // dibujar particulas
    if (particleSystem) {
        window.draw(*particleSystem);
    }

    // dibujar el logo
    window.draw(logoSprite);

    // dibujar botones
    for (auto& button : buttons) {
        button->draw(window);
    }
}



// libera recursos del estado del menu
void MenuState::cleanup() {
    // limpiar botones
    buttons.clear();
}
