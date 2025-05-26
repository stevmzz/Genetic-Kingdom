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
#include "../include/Game/Systems/Pathfinding.h"
#include "Game/Enemies/Ogre.h"
#include "Game/Enemies/DarkElves.h"
#include "Game/Enemies/Harpy.h"
#include "Game/Enemies/Mercenary.h"

// constructor del estado de juego
GameplayState::GameplayState()
    :   enemiesKilled(0),
        gameOver(false),
        selectedTowerType(TowerType::Archer),
        musicPaused(false) {
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

    // cargar sonidos del juego
    loadGameplaySounds();

    // iniciar musica
    startGameplayMusic();

    // detener la musica del menu
    game->getAudioSystem().stopAllMusic();

    // cargar fondo
    loadBackgroundTexture();

    // inicializar la cuadricula centrada en la pantalla
    float gridX = (window.getSize().x - GRID_COLS * CELL_SIZE) / 2;
    float gridY = (window.getSize().y - GRID_ROWS * CELL_SIZE) / 2;
    gameGrid = std::make_unique<Grid>(gridX, gridY, GRID_ROWS, GRID_COLS, CELL_SIZE);

    // inicializar puntos de spawn y objetivo
    initializeSpawnAndGoalPoints();

    // texto que avisa al jugador si no tiene suficiente dinero
    // siempre esta ahi, solo se muestra cuando es necesario
    insufficientGoldText.setFont(game->getFont());
    insufficientGoldText.setCharacterSize(30);
    insufficientGoldText.setFillColor(sf::Color::White);
    insufficientGoldText.setString("Not enough gold!");
    sf::FloatRect textBounds = insufficientGoldText.getLocalBounds();
    insufficientGoldText.setPosition(
        (game->getWindow().getSize().x - textBounds.width) / 2.f,
        game->getWindow().getSize().y - textBounds.height - 250.f
    );

    // texto que avisa al jugador si bloqueará el camino
    pathBlockedText.setFont(game->getFont());
    pathBlockedText.setCharacterSize(30);
    pathBlockedText.setFillColor(sf::Color::Red);
    pathBlockedText.setString("Cannot block enemy path!");
    sf::FloatRect pathTextBounds = pathBlockedText.getLocalBounds();
    pathBlockedText.setPosition(
        (game->getWindow().getSize().x - pathTextBounds.width) / 2.f,
        game->getWindow().getSize().y - pathTextBounds.height - 250.f
    );

    // font para celdas de las torres
    Tower::setSharedFont(game->getFont());

    // Font para dano encima de enemigos
    Enemy::setSharedFont(game->getFont());

    // inicializar el sistema genetico (población 20, tasa de mutación 0.1, tasa de crossover 0.7)
    geneticsSystem = std::make_unique<Genetics>(20, 0.1f, 0.7f);

    // crear un path inicial para calcular la longitud total
    auto initialPath = Pathfinding::findPath(gameGrid.get(), spawnPoint, goalPoint);

    // inicializar el gestor de oleadas
    waveManager = std::make_unique<WaveManager>(initialPath, gameGrid.get(), goalPoint, 1.5f);

    // preparar cromosomas para la primera oleada
    DynamicArray<Chromosome> firstWaveChromosomes = geneticsSystem->getChromosomesForWave(1);
    waveManager->setWaveChromosomes(firstWaveChromosomes);

    // iniciar la primera oleada automaticamente
    waveManager->startNextWave();
}




// metodo para cargar sonidos
void GameplayState::loadGameplaySounds() {

    // sonido de muerte de enemigos
    if (!game->getAudioSystem().loadSound("death", "assets/audio/death.wav")) {
        std::cerr << "error: no se pudo cargar death.mp3" << std::endl;
    }

    // sonido de colocacion de torre
    if (!game->getAudioSystem().loadSound("colocarTorre", "assets/audio/colocarTorre.wav")) {
        std::cerr << "error: no se pudo cargar colocarTorre.mp3" << std::endl;
    }

    // sonido de upgrade de torre
    if (!game->getAudioSystem().loadSound("upgrade", "assets/audio/upgrade.mp3")) {
        std::cerr << "error: no se pudo cargar upgrade.mp3" << std::endl;
    }
}



// metodo para iniciar la musica de fondo
void GameplayState::startGameplayMusic() {
    std::cout << "Iniciando música de gameplay..." << std::endl;

    if (!game->getAudioSystem().playMusicIfNotPlaying("assets/audio/gameplay-theme.mp3", true)) {
        std::cerr << "error: no se pudo cargar gameplay-theme.mp3" << std::endl;
    }
}



// pausar musica
void GameplayState::pauseMusic() {
    if (!musicPaused) {
        game->getAudioSystem().stopAllMusic();
        musicPaused = true;
    }
}



// continuar musica
void GameplayState::resumeMusic() {
    if (musicPaused) {
        startGameplayMusic();
        musicPaused = false;
        std::cout << "Música reanudada" << std::endl;
    }
}



// parar musica
void GameplayState::stopMusic() {
    game->getAudioSystem().stopAllMusic();
    musicPaused = false;
    std::cout << "Música detenida" << std::endl;
}



// cargar fondo
bool GameplayState::loadBackgroundTexture() {
    auto& window = game->getWindow();
    greenBackground.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    greenBackground.setFillColor(sf::Color(34, 139, 34)); // Verde bosque

    if (!backgroundTexture.loadFromFile("assets/images/backgrounds/backgroundGame.png")) {
        std::cerr << "error: No se pudo cargar gameplay_background.png" << std::endl;
        backgroundLoaded = false;
        return false;
    }

    // configurar el sprite del fondo png
    backgroundSprite.setTexture(backgroundTexture);

    // obtener tamaños
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();

    // escalar para cubrir toda la pantalla
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);

    backgroundLoaded = true;
    return true;
}



// inicializa el punto de entrada y salida
void GameplayState::initializeSpawnAndGoalPoints() {
    float gridStartX = gameGrid->getX();
    float gridStartY = gameGrid->getY();
    int rows = gameGrid->getRows();
    int cols = gameGrid->getCols();
    float cellSize = gameGrid->getCellSize();

    // calcular fila central
    int middleRow = rows / 2;

    // punto de entrada (borde izquierdo, fila central)
    spawnPoint = sf::Vector2f(gridStartX - cellSize/2, gridStartY + middleRow * cellSize + cellSize/2);

    // punto de salida (borde derecho, fila central)
    goalPoint = sf::Vector2f(gridStartX + (cols + 1) * cellSize + cellSize / 2, gridStartY + middleRow * cellSize + cellSize / 2);
}




// verificar si se puede colocar una torre
bool GameplayState::canPlaceTowerAt(Cell* cell) {
    if (!cell || cell->hasTower()) {
        return false;
    }

    // temporalmente "colocar" una torre para verificar
    cell->placeTower(std::make_shared<Archer>());

    // verificar que todavía existe un camino desde spawn a goal
    bool hasPath = Pathfinding::hasValidPath(gameGrid.get(), spawnPoint, goalPoint);

    // verificar que todos los enemigos activos pueden llegar al goal
    if (hasPath) {
        for (const auto& enemy : enemies) {
            if (enemy->isAlive()) {
                auto enemyPath = Pathfinding::findPath(gameGrid.get(), enemy->getPosition(), goalPoint);
                if (enemyPath.empty()) {
                    hasPath = false;
                    break;
                }
            }
        }
    }

    // verificar que futuros enemigos que aparezcan tendrán camino
    if (hasPath && waveManager->isWaveInProgress()) {
        auto spawnPath = Pathfinding::findPath(gameGrid.get(), spawnPoint, goalPoint);
        if (spawnPath.empty()) {
            hasPath = false;
        }
    }

    // remover la torre temporal
    cell->placeTower(nullptr);
    return hasPath;
}



// recalcular paths de enemigos
void GameplayState::recalculateEnemyPaths() {
    for (auto& enemy : enemies) {
        if (enemy->isAlive()) {
            auto newPath = Pathfinding::findPath(gameGrid.get(), enemy->getPosition(), goalPoint);
            if (!newPath.empty()) {
                enemy->setPath(newPath);
            }
        }
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
            recalculateEnemyPaths();
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
            float offsetY = -70.0f;
            float buttonWidth = 150.f;
            float buttonHeight = 55.f;

            // Botón Archer
            auto btnArcher = std::make_shared<Button>(
                (basePos.x- 25.0f) - 160.f, basePos.y + offsetY,
                buttonWidth, buttonHeight,
                game->getFont(),
                "ARCHER",
                [this]() {
                    if (selectedCellForPlacement) {
                        if (!canPlaceTowerAt(selectedCellForPlacement)) {
                            showPathBlocked = true;
                            pathBlockedClock.restart();
                        } else {
                            auto tower = std::make_shared<Archer>();
                            if (playerGold >= tower->getCost()) {
                                selectedCellForPlacement->placeTower(tower);
                                playerGold -= tower->getCost();
                                recalculateEnemyPaths();
                                game->getAudioSystem().playSound("colocarTorre");
                            } else {
                                showGoldWarning = true;
                                goldWarningClock.restart();
                            }
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                }
            );

            // Aplicar texturas café/madera
            btnArcher->setTextures(
                "assets/images/buttons/wood_button_normal.png",
                "assets/images/buttons/wood_button_selected.png"
            );

            auto priceTextArcher = std::make_shared<sf::Text>();
            priceTextArcher->setFont(game->getFont());
            priceTextArcher->setCharacterSize(14);
            priceTextArcher->setFillColor(sf::Color::White);
            Archer tempArcher;
            priceTextArcher->setString(std::to_string(tempArcher.getCost()) + "g");
            priceTextArcher->setPosition((basePos.x- 25.0f) - 160.f + buttonWidth/2 - 15.f, basePos.y + offsetY + buttonHeight + 5.f);
            towerPriceTexts.push_back(priceTextArcher);

            // Botón Mage
            auto btnMage = std::make_shared<Button>(
                (basePos.x- 25.0f), basePos.y + offsetY,
                buttonWidth, buttonHeight,
                game->getFont(),
                "MAGE",
                [this]() {
                    if (selectedCellForPlacement) {
                        if (!canPlaceTowerAt(selectedCellForPlacement)) {
                            showPathBlocked = true;
                            pathBlockedClock.restart();
                        } else {
                            auto tower = std::make_shared<Mage>();
                            if (playerGold >= tower->getCost()) {
                                selectedCellForPlacement->placeTower(tower);
                                playerGold -= tower->getCost();
                                recalculateEnemyPaths();
                                game->getAudioSystem().playSound("colocarTorre");
                            } else {
                                showGoldWarning = true;
                                goldWarningClock.restart();
                            }
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                }
            );

            // Aplicar texturas café/madera
            btnMage->setTextures(
                "assets/images/buttons/wood_button_normal.png",
                "assets/images/buttons/wood_button_selected.png"
            );

            auto priceTextMage = std::make_shared<sf::Text>();
            priceTextMage->setFont(game->getFont());
            priceTextMage->setCharacterSize(14);
            priceTextMage->setFillColor(sf::Color::White);
            Mage tempMage;
            priceTextMage->setString(std::to_string(tempMage.getCost()) + "g");
            priceTextMage->setPosition((basePos.x- 35.0f) + buttonWidth/2 - 15.f, basePos.y + offsetY + buttonHeight + 5.f);
            towerPriceTexts.push_back(priceTextMage);

            // Botón Gunner
            auto btnGunner = std::make_shared<Button>(
                (basePos.x- 25.0f) + 160.f, basePos.y + offsetY,
                buttonWidth, buttonHeight,
                game->getFont(),
                "GUNNER",
                [this]() {
                    if (selectedCellForPlacement) {
                        if (!canPlaceTowerAt(selectedCellForPlacement)) {
                            showPathBlocked = true;
                            pathBlockedClock.restart();
                        } else {
                            auto tower = std::make_shared<Gunner>();
                            if (playerGold >= tower->getCost()) {
                                selectedCellForPlacement->placeTower(tower);
                                playerGold -= tower->getCost();
                                recalculateEnemyPaths();
                                game->getAudioSystem().playSound("colocarTorre");
                            } else {
                                showGoldWarning = true;
                                goldWarningClock.restart();
                            }
                        }
                        selectedCellForPlacement = nullptr;
                        towerButtons.clear();
                        towerPriceTexts.clear();
                        showUpgradeGoldText = false;
                    }
                }
            );

            // Aplicar texturas café/madera
            btnGunner->setTextures(
                "assets/images/buttons/wood_button_normal.png",
                "assets/images/buttons/wood_button_selected.png"
            );

            auto priceTextGunner = std::make_shared<sf::Text>();
            priceTextGunner->setFont(game->getFont());
            priceTextGunner->setCharacterSize(14);
            priceTextGunner->setFillColor(sf::Color::White);
            Gunner tempGunner;
            priceTextGunner->setString(std::to_string(tempGunner.getCost()) + "g");
            priceTextGunner->setPosition((basePos.x- 30.0f) + 160.f + buttonWidth/2 - 20.f, basePos.y + offsetY + buttonHeight + 5.f);
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
                sf::Vector2f btnSize(170.f, 50.f);

                auto btnUpgrade = std::make_shared<Button>(
                    (basePos.x - btnSize.x) + 245.0f / 2.f, basePos.y - 85.f,
                    btnSize.x, btnSize.y,
                    game->getFont(),
                    "UPGRADE",
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

                // Aplicar texturas café/madera al botón de upgrade
                btnUpgrade->setTextures(
                    "assets/images/buttons/wood_button_normal.png",
                    "assets/images/buttons/wood_button_selected.png"
                );

                towerButtons.push_back(btnUpgrade);

                upgradeGoldText.setFont(game->getFont());
                upgradeGoldText.setCharacterSize(16);
                upgradeGoldText.setFillColor(sf::Color::White);
                upgradeGoldText.setString(std::to_string(tower->getUpgradeCost()) + "g");

                // posición centrada debajo del botón
                sf::Vector2f btnPos((basePos.x - btnSize.x) + 235.0f / 2.f, basePos.y - 85.f);
                sf::FloatRect textBounds = upgradeGoldText.getLocalBounds();

                upgradeGoldText.setPosition(
                    btnPos.x + (btnSize.x / 2.f) - (textBounds.width / 2.f), // centrar horizontalmente
                    btnPos.y + btnSize.y + 8.f // justo debajo del botón
                );

                showUpgradeGoldText = true;
            }
        }
    }
}




// actualiza la logica del estado de juego
void GameplayState::update(float dt) {

    if (!musicPaused && !game->getAudioSystem().isMusicPlaying()) {
        std::cout << "La música se detuvo inesperadamente, reiniciando..." << std::endl;
        startGameplayMusic();
    }

    // si el juego ha terminado, no hacer nada
    if (gameOver) {
        return;
    }

    // actualizar el gestor de oleadas
    auto newEnemies = waveManager->update(dt);

    // añadir los nuevos enemigos a la lista con paths calculados
    for (auto& enemy : newEnemies) {
        // calcular el path inicial para el enemigo
        auto path = Pathfinding::findPath(gameGrid.get(), enemy->getPosition(), goalPoint);
        if (!path.empty()) {
            enemy->setPath(path);
        }
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

            // sonido de muerte
            game->getAudioSystem().playSound("death");

            playerGold += (*it)->getGoldReward(); // ganar oro al matar
            std::cout << "Gold + " << (*it)->getGoldReward() << " (Total: " << playerGold << ")\n";

            // incrementar contador de enemigos eliminados
            enemiesKilled++;

            // eliminar el enemigo de la lista
            size_t index = it - enemies.begin();
            enemies.erase(index);
            it = enemies.begin() + index;        }

        else if ((*it)->hasReachedEnd()) {
            gameOver = true;
            ++it;
        }

        else {
            ++it;
        }
    }

    // Hace que las torres ataquen los enemigos
    handleTowerAttacks(dt);

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
    DynamicArray<bool> reachedEnd = waveManager->getEnemiesReachedEnd();
    DynamicArray<float> distancesTraveled = waveManager->getDistancesTraveled();
    DynamicArray<float> timesAlive = waveManager->getTimesAlive();

    // creamos un vector de ceros con el mismo tamaño
    DynamicArray<float> damagesDealt;
    damagesDealt.resize(reachedEnd.size());
    for (size_t i = 0; i < reachedEnd.size(); i++) {
        damagesDealt[i] = 0.0f;
    }

    // evaluar la población
    geneticsSystem->evaluatePopulation(reachedEnd, distancesTraveled, damagesDealt, timesAlive);

    // crear la siguiente generación
    geneticsSystem->createNextGeneration();

    // obtener cromosomas para la siguiente oleada
    DynamicArray<Chromosome> nextWaveChromosomes = geneticsSystem->getChromosomesForWave(waveManager->getEnemiesPerWave());

    // configurar el WaveManager con los nuevos cromosomas
    waveManager->setWaveChromosomes(nextWaveChromosomes);

    // iniciar la siguiente oleada
    waveManager->startNextWave();
}



// dibuja el contenido del estado de juego en la ventana
void GameplayState::render(sf::RenderWindow& window) {

    // dibujar fondo verde
    window.draw(greenBackground);

    // dibujar la cuadricula
    if (gameGrid) {
        gameGrid->draw(window);
    }

    // dibujar el fondo
    window.draw(backgroundSprite);

    // dibujar proyectiles de torres
    for (const auto& row : gameGrid->getCells()) {
        for (const auto& cell : row) {
            if (cell.hasTower()) {
                auto tower = cell.getTower();
                if (tower->type() == "Archer") {
                    std::dynamic_pointer_cast<Archer>(tower)->drawProjectiles(window);
                }
                if (tower->type() == "Mage") {
                    std::dynamic_pointer_cast<Mage>(tower)->drawProjectiles(window);
                }
                if (tower->type() == "Gunner") {
                    std::dynamic_pointer_cast<Gunner>(tower)->drawProjectiles(window);
                }
            }
        }
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
        instructionText.setString("press any key");

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

    // flashing text cuando el jugador bloqueará el camino
    if (showPathBlocked) {
        float t = pathBlockedClock.getElapsedTime().asSeconds();

        // mostrar solo durante 2 segundos
        if (t < 2.0f) {
            // efecto de parpadeo
            if (static_cast<int>(t * 5) % 2 == 0) {
                window.draw(pathBlockedText);
            }
        } else {
            showPathBlocked = false;
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
    goldText.setFillColor(sf::Color(255, 215, 0));

    goldText.setString("GOLD: " + std::to_string(playerGold));
    goldText.setPosition(20.f, 20.f); // esquina superior izquierda

    window.draw(goldText);

}

// libera recursos si es necesario cuando se sale del estado
void GameplayState::cleanup() {
    stopMusic();
    gameGrid.reset();
    enemies.clear();
    waveManager.reset();
    geneticsSystem.reset();
}

void GameplayState::handleTowerAttacks(float dt) {
    const auto& cellGrid = gameGrid->getCells(); // get 2D vector of cells

    for (const auto& row : cellGrid) {
        for (const auto& cell : row) {
            if (cell.hasTower()) {
                auto tower = cell.getTower();

                if (tower->type() == "Archer") {
                    std::dynamic_pointer_cast<Archer>(tower)->updateProjectiles(dt);
                }
                if (tower->type() == "Mage") {
                    std::dynamic_pointer_cast<Mage>(tower)->updateProjectiles(dt);
                }
                if (tower->type() == "Gunner") {
                    std::dynamic_pointer_cast<Gunner>(tower)->updateProjectiles(dt);
                }

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
