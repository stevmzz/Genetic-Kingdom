#include "../include/States/GameplayState.h"
#include "../include/States/PauseState.h"
#include "../include/Core/Game.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <States/MenuStates/MenuState.h>
#include "Game/Towers/Archer.h"
#include "Game/Towers/Mage.h"
#include "Game/Towers/Gunner.h"

//Incluye la clase del panel de estadisticas
#include "../include/UI/StatsPanel.h"


// constructor del estado de juego
GameplayState::GameplayState()
    :   enemiesKilled(0),
        gameOver(false),
        selectedTowerType(TowerType::Archer){
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

    // texto que avisa al jugador si no tiene suficiente dinero
    // siempre esta ahi, solo se muestra cuando es necesario
    insufficientGoldText.setFont(game->getFont());
    insufficientGoldText.setCharacterSize(20);
    insufficientGoldText.setFillColor(sf::Color::Red);
    insufficientGoldText.setString("Not enough gold!");
    sf::FloatRect textBounds = insufficientGoldText.getLocalBounds();
    insufficientGoldText.setPosition(
        (game->getWindow().getSize().x - textBounds.width) / 2.f,
        game->getWindow().getSize().y - textBounds.height - 100.f
    );

    // font para celdas de las torres
    Tower::setSharedFont(game->getFont());

    // inicializar el sistema genetico (población 20, tasa de mutación 0.1, tasa de crossover 0.7)
    geneticsSystem = std::make_unique<Genetics>(20, 0.1f, 0.7f);

    // inicializar el gestor de oleadas (camino, intervalo entre enemigos)
    waveManager = std::make_unique<WaveManager>(testPath, 1.5f);

    // preparar cromosomas para la primera oleada
    std::vector<Chromosome> firstWaveChromosomes = geneticsSystem->getChromosomesForWave(1);
    waveManager->setWaveChromosomes(firstWaveChromosomes);

    // iniciar la primera oleada automaticamente
    waveManager->startNextWave();

    // inicializar el panel de estadísticas
    statsPanel = std::make_unique<StatsPanel>(game->getFont());

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

    // primero, si hay botones activos, revisar si se hizo clic en ellos
    for (auto& button : towerButtons) {
        if (button->handleEvent(event)) {
            return; // solo salir si realmente se hizo clic en un botón
        }
    }

    // cancelar selección si se hace clic fuera de botones y celda activa
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left &&
        selectedCellForPlacement) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));

        if (clickedOutsideButtonsAndSelectedCell(mousePos)) {
            selectedCellForPlacement = nullptr;
            towerButtons.clear();
            towerPriceTexts.clear();
            showUpgradeGoldText = false;
            return;
        }
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

        gameGrid->clearSelection();

        // Evitar que brillen las celdas si hay botones activos
        if (towerButtons.empty()) {
            Cell* hoveredCell = gameGrid->getCellAtPosition(mouseX, mouseY);
            if (hoveredCell) {
                hoveredCell->setSelected(true);
            }
        }
    }

    // si ya hay una celda esperando colocación, no permitir seleccionar otra
    if (selectedCellForPlacement && !towerButtons.empty()) {
        return;
    }

    // clic izquierdo: mostrar botones de torre si la celda es válida
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left &&
        gameGrid) {

        float mouseX = static_cast<float>(event.mouseButton.x);
        float mouseY = static_cast<float>(event.mouseButton.y);

        Cell* clickedCell = gameGrid->getCellAtPosition(mouseX, mouseY);
        if (clickedCell && !clickedCell->isPathCell() && !clickedCell->hasTower()) {
            selectedCellForPlacement = clickedCell;
            towerButtons.clear();
            towerPriceTexts.clear();
            showUpgradeGoldText = false;

            sf::Vector2f basePos = clickedCell->getPosition();
            float offsetY = -50.0f;
            sf::Vector2f size(80.f, 40.f);

            // Botón Archer
            auto btnArcher = std::make_shared<Button>(
                (basePos.x- 25.0f) - 140.f, basePos.y + offsetY,
                125.f, 75.f,
                game->getFont(),
                "Archer",
                [this]() {
                    if (selectedCellForPlacement) {
                        auto tower = std::make_shared<Archer>();
                        if (playerGold >= tower->getCost()) {
                            selectedCellForPlacement->placeTower(tower);
                            playerGold -= tower->getCost();
                        } else {
                            showGoldWarning = true;
                            goldWarningClock.restart();
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                }
            );
            auto priceTextArcher = std::make_shared<sf::Text>();
            priceTextArcher->setFont(game->getFont());
            priceTextArcher->setCharacterSize(16);
            priceTextArcher->setFillColor(sf::Color::Yellow);
            Archer tempArcher;
            priceTextArcher->setString(std::to_string(tempArcher.getCost()) + "g");
            priceTextArcher->setPosition(btnArcher->getPosition().x + 40.f, btnArcher->getPosition().y + 80.f);
            towerPriceTexts.push_back(priceTextArcher);

            // Botón Mage
            auto btnMage = std::make_shared<Button>(
                (basePos.x- 25.0f), basePos.y + offsetY,
                125.f, 75.f,
                game->getFont(),
                "Mage",
                [this]() {
                    if (selectedCellForPlacement) {
                        auto tower = std::make_shared<Mage>();
                        if (playerGold >= tower->getCost()) {
                            selectedCellForPlacement->placeTower(tower);
                            playerGold -= tower->getCost();
                        } else {
                            showGoldWarning = true;
                            goldWarningClock.restart();
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                }
            );

            auto priceTextMage = std::make_shared<sf::Text>();
            priceTextMage->setFont(game->getFont());
            priceTextMage->setCharacterSize(16);
            priceTextMage->setFillColor(sf::Color::Yellow);
            Mage tempMage;
            priceTextMage->setString(std::to_string(tempMage.getCost()) + "g");
            priceTextMage->setPosition(btnMage->getPosition().x + 30.f, btnMage->getPosition().y + 80.f);
            towerPriceTexts.push_back(priceTextMage);

            // Botón Gunner
            auto btnGunner = std::make_shared<Button>(
                (basePos.x- 25.0f) + 140.f, basePos.y + offsetY,
                125.f, 75.f,
                game->getFont(),
                "Gunner",
                [this]() {
                    if (selectedCellForPlacement) {
                        auto tower = std::make_shared<Gunner>();
                        if (playerGold >= tower->getCost()) {
                            selectedCellForPlacement->placeTower(tower);
                            playerGold -= tower->getCost();
                        } else {
                            showGoldWarning = true;
                            goldWarningClock.restart();
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                }
            );

            auto priceTextGunner = std::make_shared<sf::Text>();
            priceTextGunner->setFont(game->getFont());
            priceTextGunner->setCharacterSize(16);
            priceTextGunner->setFillColor(sf::Color::Yellow);
            Gunner tempGunner;
            priceTextGunner->setString(std::to_string(tempGunner.getCost()) + "g");
            priceTextGunner->setPosition(btnGunner->getPosition().x + 30.f, btnGunner->getPosition().y + 80.f);
            towerPriceTexts.push_back(priceTextGunner);


            towerButtons.push_back(btnArcher);
            towerButtons.push_back(btnMage);
            towerButtons.push_back(btnGunner);
        }
        else if (clickedCell && clickedCell->hasTower()) {
            selectedCellForPlacement = clickedCell;
            towerButtons.clear();
            towerPriceTexts.clear();
            showUpgradeGoldText = false;

            std::shared_ptr<Tower> tower = clickedCell->getTower();
            if (tower && tower->canUpgrade()) {
                int upgradeCost = tower->getUpgradeCost();

                sf::Vector2f basePos = clickedCell->getPosition();
                sf::Vector2f btnSize(140.f, 60.f);

                auto btnUpgrade = std::make_shared<Button>(
                    (basePos.x - btnSize.x) + 215.0f / 2.f, basePos.y - 75.f,
                    btnSize.x, btnSize.y,
                    game->getFont(),
                    "Upgrade",
                    [tower, this]() {
                        if (tower && tower->canUpgrade() && playerGold >= tower->getUpgradeCost()) {
                            playerGold -= tower->getUpgradeCost();
                            tower->upgrade();
                            std::cout << "Tower upgraded! New level: " << tower->getLevel()
                                      << ", Remaining gold: " << playerGold << "\n";
                        } else {
                            showGoldWarning = true;
                            goldWarningClock.restart();
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                );
                towerButtons.push_back(btnUpgrade);

                upgradeGoldText.setFont(game->getFont());
                upgradeGoldText.setCharacterSize(16);
                upgradeGoldText.setFillColor(sf::Color::Yellow);
                upgradeGoldText.setString(std::to_string(tower->getUpgradeCost()) + "g");

                // posición centrada debajo del botón
                sf::Vector2f btnPos((basePos.x - btnSize.x) + 215.0f / 2.f, basePos.y - 75.f);
                sf::FloatRect textBounds = upgradeGoldText.getLocalBounds();

                upgradeGoldText.setPosition(
                    btnPos.x + (btnSize.x / 2.f) - (textBounds.width / 2.f), // centrar horizontalmente
                    btnPos.y + btnSize.y + 5.f // justo debajo del botón
                );

                showUpgradeGoldText = true;
            }
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

            playerGold += (*it)->getGoldReward(); // ganar oro al matar
            std::cout << "Gold + " << (*it)->getGoldReward() << " (Total: " << playerGold << ")\n";

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

    // Hace que las torres ataquen los enemigos
    handleTowerAttacks();

    // si el juego no ha terminado, comprobar si la oleada ha terminado
    if (!gameOver) {
        if (enemies.empty() && !waveManager->isWaveInProgress() && waveManager->getEnemiesSpawned() > 0) {
            prepareNextGeneration();
        }
    }

    //Declara como el panel de estadisticas se va actualizando durante la partida
    std::vector<float> fitnessList = geneticsSystem->getCurrentFitnessScores();
    std::vector<int> towerLevels;

    const auto& cells = gameGrid->getCells();
    for (const auto& row : cells) {
        for (const auto& cell : row) {
            if (cell.hasTower()) {
                towerLevels.push_back(cell.getTower()->getLevel());
            }
        }
    }

    statsPanel->update(
        geneticsSystem->getGenerationCount(),
        enemiesKilled,
        fitnessList,
        towerLevels,
        geneticsSystem->getMutationRate(),
        geneticsSystem->getMutationCount()
    );

    if (gameOver) {
        statsPanel->setVisible(false);
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

    // dibujar botones de opciones de torres al hacer click en celda
    for (const auto& button : towerButtons) {
        button->draw(window);
    }

    // agregar los textos de los precios de las torres
    for (const auto& text : towerPriceTexts) {
        window.draw(*text);
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

    // flashing text cuando el jugador no tiene suficiente oro
    if (showGoldWarning) {
        float t = goldWarningClock.getElapsedTime().asSeconds();

        // mostrar solo durante 2 segundos
        if (t < 2.0f) {
            // efecto de parpadeo
            if (static_cast<int>(t * 5) % 2 == 0) {
                window.draw(insufficientGoldText);
            }
        } else {
            showGoldWarning = false;
        }
    }

    // muestra la cantidad de oro para mejorar la torre
    if (showUpgradeGoldText) {
        window.draw(upgradeGoldText);
    }

    // Muestra el oro en pantalla
    sf::Text goldText;
    goldText.setFont(game->getFont());
    goldText.setCharacterSize(20);
    goldText.setFillColor(sf::Color::Yellow);

    goldText.setString("Gold: " + std::to_string(playerGold));
    goldText.setPosition(20.f, 20.f); // esquina superior izquierda

    window.draw(goldText);

    //Dibujar el panel de estadisticas
    if (statsPanel) {
        statsPanel->draw(window);
    }
}

// libera recursos si es necesario cuando se sale del estado
void GameplayState::cleanup() {
    gameGrid.reset();
    enemies.clear();
    waveManager.reset();
    geneticsSystem.reset();
}

void GameplayState::handleTowerAttacks() {
    const auto& cellGrid = gameGrid->getCells(); // get 2D vector of cells

    for (const auto& row : cellGrid) {
        for (const auto& cell : row) {
            if (cell.hasTower()) {
                auto tower = cell.getTower();

                for (const auto& enemyPtr : enemies) {
                    if (enemyPtr->isAlive()) {
                        float dx = enemyPtr->getPosition().x - cell.getPosition().x;
                        float dy = enemyPtr->getPosition().y - cell.getPosition().y;
                        float distance = std::hypot(dx, dy);

                        if (distance <= tower->getRange()) {
                            tower->attack(*enemyPtr, enemies);
                            break; // only one attack per cycle
                        }
                    }
                }
            }
        }
    }
}

bool GameplayState::clickedOutsideButtonsAndSelectedCell(const sf::Vector2f& mousePos) const {
    if (!selectedCellForPlacement) return false;

    // Verificar si se hizo clic dentro de algún botón
    for (const auto& button : towerButtons) {
        if (button->getBounds().contains(mousePos)) {
            return false;
        }
    }

    // Verificar si se hizo clic dentro de la celda seleccionada
    float cellSize = gameGrid->getCellSize();
    sf::FloatRect selectedCellBounds(
        selectedCellForPlacement->getPosition().x - cellSize / 2.f,
        selectedCellForPlacement->getPosition().y - cellSize / 2.f,
        cellSize, cellSize
    );

    if (selectedCellBounds.contains(mousePos)) {
        return false;
    }

    return true; // el clic fue fuera de todo
}
