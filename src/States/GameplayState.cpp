#include "../include/States/GameplayState.h"
#include "../include/States/PauseState.h"
#include "../include/Core/Game.h"
#include <iostream>
#include <sstream>

// constructor del estado de juego
GameplayState::GameplayState() {
}



// destructor del estado de juego
GameplayState::~GameplayState() {
}



// inicializa el estado de juego
void GameplayState::init() {
    auto& window = game->getWindow();
    auto& font = game->getFont();

    // pone el cursor visible
    game->getWindow().setMouseCursorVisible(true);

    // detener la musica del menu
    game->getAudioSystem().stopAllMusic();

    // inicializar la cuadricula centrada en la pantalla
    float gridX = (window.getSize().x - GRID_COLS * CELL_SIZE) / 2;
    float gridY = (window.getSize().y - GRID_ROWS * CELL_SIZE) / 2;
    gameGrid = std::make_unique<Grid>(gridX, gridY, GRID_ROWS, GRID_COLS, CELL_SIZE);

    // crear un camino de prueba para los enemigos
    createTestPath();

    // inicializar el gestor de oleadas
    waveManager = std::make_unique<WaveManager>(testPath, 1.5f);
}



// crea un camino de prueba para los enemigos
void GameplayState::createTestPath() {
    // obtener informacion de la cuadricula
    float gridStartX = gameGrid->getX();
    float gridStartY = gameGrid->getY();
    int rows = gameGrid->getRows();
    int cols = gameGrid->getCols();
    float cellSize = gameGrid->getCellSize();

    // calcular fila central
    int middleRow = rows / 2;

    // punto de entrada (borde izquierdo, fila central)
    sf::Vector2f entryPoint(gridStartX - cellSize/2, gridStartY + middleRow * cellSize + cellSize/2);

    // punto de salida (borde derecho, fila central)
    sf::Vector2f exitPoint(gridStartX + cols * cellSize + cellSize/2, gridStartY + middleRow * cellSize + cellSize/2);

    // limpiar el camino anterior
    testPath.clear();

    // anadir punto de entrada
    testPath.push_back(entryPoint);

    // anadir puntos a lo largo del camino recto
    for (int i = 0; i <= cols; i++) {
        sf::Vector2f pathPoint(gridStartX + i * cellSize, gridStartY + middleRow * cellSize + cellSize/2);
        testPath.push_back(pathPoint);
    }

    // anadir punto de salida
    testPath.push_back(exitPoint);

    // marcar las celdas del camino como no disponibles para colocar torres
    for (int i = 0; i < cols; i++) {
        Cell* pathCell = gameGrid->getCellAt(middleRow, i);
        if (pathCell) {
            pathCell->setIsPath(true);
        }
    }
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

        // encontrar la celda bajo el mouse
        Cell* hoveredCell = gameGrid->getCellAtPosition(mouseX, mouseY);
        if (hoveredCell) {
            hoveredCell->setSelected(true);
        }
    }
}



// actualiza la logica del estado de juego
void GameplayState::update(float dt) {
    // actualizar el gestor de oleadas
    auto newEnemies = waveManager->update(dt);

    // anadir los nuevos enemigos a la lista
    for (auto& enemy : newEnemies) {
        enemies.push_back(std::move(enemy));
    }

    // actualizar cada enemigo
    for (auto it = enemies.begin(); it != enemies.end();) {
        (*it)->update(dt);

        // eliminar enemigos muertos o que hayan llegado al final
        if (!(*it)->isAlive() || (*it)->hasReachedEnd()) {
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}



// dibuja el contenido del estado de juego en la ventana
void GameplayState::render(sf::RenderWindow& window) {
    // dibujar el fondo
    window.draw(backgroundSprite);

    // dibujar la cuadricula
    if (gameGrid) {
        gameGrid->draw(window);
    }

    // dibujar enemigos
    for (const auto& enemy : enemies) {
        window.draw(*enemy);
    }

    // dibujar el texto de informacion de la oleada
    window.draw(waveInfoText);
}



// libera recursos si es necesario cuando se sale del estado
void GameplayState::cleanup() {
    gameGrid.reset();
    enemies.clear();
    waveManager.reset();
}
