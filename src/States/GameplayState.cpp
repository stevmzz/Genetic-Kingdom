#include "../include/States/GameplayState.h"
#include "../include/States/PauseState.h"
#include "../include/Core/Game.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <States/MenuStates/MenuState.h>

// constructor del estado de juego
GameplayState::GameplayState()
    :   enemiesKilled(0),
        gameOver(false) {
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

    // inicializar el sistema genetico (población 20, tasa de mutación 0.1, tasa de crossover 0.7)
    geneticsSystem = std::make_unique<Genetics>(20, 0.1f, 0.7f);

    // inicializar el gestor de oleadas (camino, intervalo entre enemigos)
    waveManager = std::make_unique<WaveManager>(testPath, 1.5f);

    // preparar cromosomas para la primera oleada
    std::vector<Chromosome> firstWaveChromosomes = geneticsSystem->getChromosomesForWave(1);
    waveManager->setWaveChromosomes(firstWaveChromosomes);

    // iniciar la primera oleada automaticamente
    waveManager->startNextWave();
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

    // añadir punto de entrada
    testPath.push_back(entryPoint);

    // añadir puntos a lo largo del camino recto
    for (int i = 0; i <= cols; i++) {
        sf::Vector2f pathPoint(gridStartX + i * cellSize, gridStartY + middleRow * cellSize + cellSize/2);
        testPath.push_back(pathPoint);
    }

    // añadir punto de salida
    testPath.push_back(exitPoint);

    // marcar las celdas del camino como no disponibles para colocar torres
    for (int i = 0; i < cols; i++) {
        Cell* pathCell = gameGrid->getCellAt(middleRow, i);
        if (pathCell) {
            pathCell->setIsPath(true);
        }
    }

    // calcular la longitud total del camino y pasarla al WaveManager
    float totalLength = 0.0f;
    if (testPath.size() > 1) {
        for (size_t i = 0; i < testPath.size() - 1; i++) {
            sf::Vector2f segment = testPath[i+1] - testPath[i];
            totalLength += std::sqrt(segment.x * segment.x + segment.y * segment.y);
        }
    }

    if (waveManager) {
        waveManager->setPathTotalLength(totalLength);
    }
}



// maneja los eventos del estado de juego
void GameplayState::handleEvents(sf::Event& event) {
    // si el juego ha terminado, solo permitir salir
    if (gameOver) {
        if (event.type == sf::Event::KeyPressed) {
            auto menuState = std::make_shared<MenuState>();
            game->pushState(menuState);
        }
        return;
    }

    if (event.type == sf::Event::KeyPressed) {
        // si se presiona la tecla escape se cambia al estado de pausa
        if (event.key.code == sf::Keyboard::Escape) {
            auto pauseState = std::make_shared<PauseState>();
            game->pushState(pauseState);
        }
        // para pruebas: iniciar la siguiente oleada con 'N'
        else if (event.key.code == sf::Keyboard::N && !waveManager->isWaveInProgress()) {
            prepareNextGeneration();
        }
    }

    // manejar eventos del mouse para la cuadricula
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
    // si el juego ha terminado, no hacer nada
    if (gameOver) {
        return;
    }

    // actualizar el gestor de oleadas
    auto newEnemies = waveManager->update(dt);

    // añadir los nuevos enemigos a la lista
    for (auto& enemy : newEnemies) {
        enemies.push_back(std::move(enemy));
    }

    // actualizar cada enemigo
    for (auto it = enemies.begin(); it != enemies.end();) {
        (*it)->update(dt);

        // comprobar el estado del enemigo
        if (!(*it)->isAlive()) {
            int id = (*it)->getId();
            float distanceTraveled = (*it)->getTotalDistanceTraveled();
            float timeAlive = (*it)->getTimeAlive();

            // registrar su rendimiento
            waveManager->trackEnemyPerformance(id, false, distanceTraveled, timeAlive);

            // incrementar contador de enemigos eliminados
            enemiesKilled++;

            // eliminar el enemigo de la lista
            it = enemies.erase(it);
        }

        else if ((*it)->hasReachedEnd()) {
            gameOver = true;
            ++it;
        }

        else {
            ++it;
        }
    }

    // si el juego no ha terminado, comprobar si la oleada ha terminado
    if (!gameOver) {
        if (enemies.empty() && !waveManager->isWaveInProgress() && waveManager->getEnemiesSpawned() > 0) {
            prepareNextGeneration();
        }
    }
}



// prepara la siguiente generacion usando el sistema genetico
void GameplayState::prepareNextGeneration() {
    // obtener datos de la oleada anterior
    std::vector<bool> reachedEnd = waveManager->getEnemiesReachedEnd();
    std::vector<float> distancesTraveled = waveManager->getDistancesTraveled();
    std::vector<float> timesAlive = waveManager->getTimesAlive();

    // creamos un vector de ceros con el mismo tamaño
    std::vector<float> damagesDealt(reachedEnd.size(), 0.0f);

    // evaluar la población
    geneticsSystem->evaluatePopulation(reachedEnd, distancesTraveled, damagesDealt, timesAlive);

    // crear la siguiente generación
    geneticsSystem->createNextGeneration();

    // obtener cromosomas para la siguiente oleada
    std::vector<Chromosome> nextWaveChromosomes = geneticsSystem->getChromosomesForWave(waveManager->getEnemiesPerWave());

    // configurar el WaveManager con los nuevos cromosomas
    waveManager->setWaveChromosomes(nextWaveChromosomes);

    // iniciar la siguiente oleada
    waveManager->startNextWave();
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

    // si el juego ha terminado, mostrar mensaje de Game Over
    if (gameOver) {
        auto& font = game->getFont();
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER");

        // centrar el texto
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setPosition(
            (window.getSize().x - textBounds.width) / 2.0f,
            window.getSize().y * 0.4f
        );

        // texto de instrucción
        sf::Text instructionText;
        instructionText.setFont(font);
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setString("presione cualquier tecla");

        // centrar el texto de instrucción
        textBounds = instructionText.getLocalBounds();
        instructionText.setPosition(
            (window.getSize().x - textBounds.width) / 2.0f,
            window.getSize().y * 0.5f
        );

        window.draw(gameOverText);
        window.draw(instructionText);
    }
}

// libera recursos si es necesario cuando se sale del estado
void GameplayState::cleanup() {
    gameGrid.reset();
    enemies.clear();
    waveManager.reset();
    geneticsSystem.reset();
}
