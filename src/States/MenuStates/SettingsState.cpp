#include "../include/States/MenuStates/SettingsState.h"
#include "../include/Core/Game.h"
#include "../include/States/MenuStates/MenuState.h"
#include <iostream>

// constructor del estado de configuracion
SettingsState::SettingsState()
    : masterVolumeSlider(nullptr),
      musicVolumeSlider(nullptr),
      sfxVolumeSlider(nullptr),
      selectedControlIndex(0) {
}



// destructor del estado de configuracion
SettingsState::~SettingsState() {
}



// inicializa el estado de configuracion
void SettingsState::init() {
    auto& window = game->getWindow();
    auto& font = game->getFont();

    // fondo
    // cargar la textura de fondo
    if (!backgroundTexture.loadFromFile("assets/images/backgrounds/background_menus.png")) {
        std::cerr << "error al cargar la imagen de fondo" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture);

    // ajustar la escala para cubrir toda la ventana
    float scaleX = window.getSize().x / static_cast<float>(backgroundTexture.getSize().x);
    float scaleY = window.getSize().y / static_cast<float>(backgroundTexture.getSize().y);
    backgroundSprite.setScale(scaleX, scaleY);

    // titulo
    settingsTitle.setFont(font);
    settingsTitle.setString("SETTINGS");
    settingsTitle.setCharacterSize(36);
    settingsTitle.setFillColor(sf::Color::White);

    // centrar el titulo
    sf::FloatRect titleRect = settingsTitle.getLocalBounds();
    settingsTitle.setPosition(
        (window.getSize().x - titleRect.width) / 2,
        window.getSize().y * 0.1f
    );

    // sliders de volumen
    // posiciones base para etiquetas y sliders
    float labelX = window.getSize().x * 0.25f;
    float sliderWidth = 300.f;
    float sliderHeight = 30.f;
    float spacingY = 80.f; // espaciado vertical entre cada grupo de controles

    float currentY = window.getSize().y * 0.25f;

    // volumen maestro
    masterVolumeLabel.setFont(font);
    masterVolumeLabel.setString("Master Volume");
    masterVolumeLabel.setCharacterSize(24);
    masterVolumeLabel.setFillColor(sf::Color::White);
    masterVolumeLabel.setPosition(labelX, currentY);

    currentY += 40.f; // espacio entre la etiqueta y el slider

    // crear el slider para el volumen maestro
    masterVolumeSlider = std::make_unique<Slider>(
        labelX, currentY,
        sliderWidth, sliderHeight,
        font, 0.0f, 100.0f, game->getMasterVolume(),
        [this](float value) {
            this->game->setMasterVolume(value);
        }
    );

    currentY += spacingY; // espacio para el siguiente control

    // volumen de musica
    musicVolumeLabel.setFont(font);
    musicVolumeLabel.setString("Music Volume");
    musicVolumeLabel.setCharacterSize(24);
    musicVolumeLabel.setFillColor(sf::Color::White);
    musicVolumeLabel.setPosition(labelX, currentY);

    currentY += 40.f;

    // crear el slider para el volumen de musica
    musicVolumeSlider = std::make_unique<Slider>(
        labelX, currentY,
        sliderWidth, sliderHeight,
        font, 0.0f, 100.0f, game->getMusicVolume(),
        [this](float value) {
            this->game->setMusicVolume(value);

            // reproducir un sonido de musica para demostrar el cambio
            // (opcional, pero util para el usuario)
        }
    );

    currentY += spacingY;

    // volumen de efectos
    sfxVolumeLabel.setFont(font);
    sfxVolumeLabel.setString("SFX Volume");
    sfxVolumeLabel.setCharacterSize(24);
    sfxVolumeLabel.setFillColor(sf::Color::White);
    sfxVolumeLabel.setPosition(labelX, currentY);

    currentY += 40.f;

    // crear el slider para efectos de sonido
    sfxVolumeSlider = std::make_unique<Slider>(
        labelX, currentY,
        sliderWidth, sliderHeight,
        font, 0.0f, 100.0f, game->getSfxVolume(),
        [this](float value) {
            this->game->setSfxVolume(value);

            // reproducir un efecto de sonido para demostrar el cambio
            if (value > 0) {
                this->game->getAudioSystem().playSound("select");
            }
        }
    );

    // botones
    float buttonWidth = 250.f;
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
        "assets/images/boton_deselected.png",
        "assets/images/boton_selected.png"
    );

    buttons.push_back(std::move(exitButton));

    // seleccionar el primer control por defecto (master volume slider)
    selectControl(0);
}



// maneja los eventos del estado de configuracion
void SettingsState::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {

            // mover hacia arriba
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if (selectedControlIndex > 0) {
                    selectControl(selectedControlIndex - 1);
                }
                break;

            // mover hacia abajo
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (selectedControlIndex < 3) {
                    selectControl(selectedControlIndex + 1);
                }
                break;

            // mover hacia la izquiera
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                if (selectedControlIndex == 0 && masterVolumeSlider) {
                    masterVolumeSlider->decrease(5.0f);
                } else if (selectedControlIndex == 1 && musicVolumeSlider) {
                    musicVolumeSlider->decrease(5.0f);
                } else if (selectedControlIndex == 2 && sfxVolumeSlider) {
                    sfxVolumeSlider->decrease(5.0f);
                }
                break;

            // mover hacia la derecha
            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                if (selectedControlIndex == 0 && masterVolumeSlider) {
                    masterVolumeSlider->increase(5.0f);
                } else if (selectedControlIndex == 1 && musicVolumeSlider) {
                    musicVolumeSlider->increase(5.0f);
                } else if (selectedControlIndex == 2 && sfxVolumeSlider) {
                    sfxVolumeSlider->increase(5.0f);
                }
                break;

            // precionar enter
            case sf::Keyboard::Return:
                if (selectedControlIndex == 3 && !buttons.empty()) {
                    buttons[0]->activate();
                }
                break;

            // salir con escape
            case sf::Keyboard::Escape:
                auto menuState = std::make_shared<MenuState>();
                game->changeState(menuState);
                break;
        }
    }
}



// actualiza la logica del estado de configuracion
void SettingsState::update(float dt) {
}



// dibuja el contenido del estado de configuracion en la ventana
void SettingsState::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    window.draw(settingsTitle);

    // dibujar etiquetas
    window.draw(masterVolumeLabel);
    window.draw(musicVolumeLabel);
    window.draw(sfxVolumeLabel);

    // dibujar sliders
    if (masterVolumeSlider) {
        window.draw(*masterVolumeSlider);
    }
    if (musicVolumeSlider) {
        window.draw(*musicVolumeSlider);
    }
    if (sfxVolumeSlider) {
        window.draw(*sfxVolumeSlider);
    }

    // dibujar botones
    for (auto& button : buttons) {
        button->draw(window);
    }
}



// libera recursos del estado de configuracion
void SettingsState::cleanup() {
    buttons.clear();
    masterVolumeSlider.reset();
    musicVolumeSlider.reset();
    sfxVolumeSlider.reset();
}



// selecciona un control por su indice
void SettingsState::selectControl(int index) {
    // deseleccionar el control actual
    if (selectedControlIndex == 0 && masterVolumeSlider) {
        masterVolumeSlider->setSelected(false);
    } else if (selectedControlIndex == 1 && musicVolumeSlider) {
        musicVolumeSlider->setSelected(false);
    } else if (selectedControlIndex == 2 && sfxVolumeSlider) {
        sfxVolumeSlider->setSelected(false);
    } else if (selectedControlIndex == 3 && !buttons.empty()) {
        buttons[0]->setSelected(false);
    }

    // actualizar el indice
    selectedControlIndex = index;

    // seleccionar el nuevo control
    if (selectedControlIndex == 0 && masterVolumeSlider) {
        masterVolumeSlider->setSelected(true);
    } else if (selectedControlIndex == 1 && musicVolumeSlider) {
        musicVolumeSlider->setSelected(true);
    } else if (selectedControlIndex == 2 && sfxVolumeSlider) {
        sfxVolumeSlider->setSelected(true);
    } else if (selectedControlIndex == 3 && !buttons.empty()) {
        buttons[0]->setSelected(true);
    }

    // reproducir sonido de seleccion
    game->getAudioSystem().playSound("select");
}