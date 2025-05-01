#include "../include/States/GameplayState.h"
#include "../include/States/PauseState.h"
#include "../include/Core/Game.h"
#include <iostream>



// constructor del estado de juego
GameplayState::GameplayState() {
}



// destructor del estado de juego
GameplayState::~GameplayState() {
}



// inicializa el estado de juego
void GameplayState::init() {
    auto& window = game->getWindow();

    // pone el cursor visible
    game->getWindow().setMouseCursorVisible(true);

    // detener la musica del menu
    game->getAudioSystem().stopAllMusic();

    // inicializar la cuadricula centrada en la pantalla
    float gridX = (window.getSize().x - GRID_COLS * CELL_SIZE) / 2;
    float gridY = (window.getSize().y - GRID_ROWS * CELL_SIZE) / 2;
    gameGrid = std::make_unique<Grid>(gridX, gridY, GRID_ROWS, GRID_COLS, CELL_SIZE);

}



// maneja los eventos del estado de juego
void GameplayState::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        // si se presiona la tecla escape se cambia al estado de pausa
        if (event.key.code == sf::Keyboard::Escape) {
            auto pauseState = std::make_shared<PauseState>();
            game->pushState(pauseState);
        }
    }

    // manejar eventos del mouse
    if (event.type == sf::Event::MouseMoved && gameGrid) {
        float mouseX = static_cast<float>(event.mouseMove.x);
        float mouseY = static_cast<float>(event.mouseMove.y);

        // deseleccionar todas las celdas
        gameGrid->clearSelection();

        // encontrar la celda bajo el mouse y seleccionarla
        Cell* hoveredCell = gameGrid->getCellAtPosition(mouseX, mouseY);
        if (hoveredCell) {
            hoveredCell->setSelected(true);
        }
    }
}



// actualiza la logica del estado de juego
void GameplayState::update(float dt) {
}



// dibuja el contenido del estado de juego en la ventana
void GameplayState::render(sf::RenderWindow& window) {
    // dibujar el fondo
    window.draw(backgroundSprite);

    // dibujar la cuadricula
    if (gameGrid) {
        gameGrid->draw(window);
    }
}



// libera recursos si es necesario cuando se sale del estado
void GameplayState::cleanup() {
    gameGrid.reset();
}
