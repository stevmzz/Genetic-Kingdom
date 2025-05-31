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
#include "../include/Game/Genetics/Genetics.h"
#include "../include/UI/StatsPanel.h"

// inicializa el estado de juego con valores por defecto
GameplayState::GameplayState()
    :   enemiesKilled(0),
        gameOver(false),
        selectedTowerType(TowerType::Archer),
        musicPaused(false),
        enemiesKilledThisWave(0),
        currentWaveForStats(0) {
}



// limpia recursos al destruir el estado
GameplayState::~GameplayState() {
}



// configura todos los sistemas necesarios para el juego
void GameplayState::init() {
    auto& window = game->getWindow();
    auto& font = game->getFont();

    // activar cursor del mouse para interacciones
    game->getWindow().setMouseCursorVisible(true);

    // preparar todos los sonidos del juego
    loadGameplaySounds();

    // iniciar musica de fondo
    startGameplayMusic();

    // detener cualquier musica previa del menu
    game->getAudioSystem().stopAllMusic();

    // configurar imagen de fondo
    loadBackgroundTexture();

    // crear cuadricula centrada en pantalla
    float gridX = (window.getSize().x - GRID_COLS * CELL_SIZE) / 2;
    float gridY = (window.getSize().y - GRID_ROWS * CELL_SIZE) / 2;
    gameGrid = std::make_unique<Grid>(gridX, gridY, GRID_ROWS, GRID_COLS, CELL_SIZE);

    // definir puntos de entrada y salida de enemigos
    initializeSpawnAndGoalPoints();

    // configurar mensaje de oro insuficiente
    insufficientGoldText.setFont(game->getFont());
    insufficientGoldText.setCharacterSize(30);
    insufficientGoldText.setFillColor(sf::Color::White);
    insufficientGoldText.setString("Not enough gold!");
    sf::FloatRect textBounds = insufficientGoldText.getLocalBounds();
    insufficientGoldText.setPosition(
        (game->getWindow().getSize().x - textBounds.width) / 2.f,
        game->getWindow().getSize().y - textBounds.height - 250.f
    );

    // configurar mensaje de bloqueo de camino
    pathBlockedText.setFont(game->getFont());
    pathBlockedText.setCharacterSize(30);
    pathBlockedText.setFillColor(sf::Color::Red);
    pathBlockedText.setString("Cannot block enemy path!");
    sf::FloatRect pathTextBounds = pathBlockedText.getLocalBounds();
    pathBlockedText.setPosition(
        (game->getWindow().getSize().x - pathTextBounds.width) / 2.f,
        game->getWindow().getSize().y - pathTextBounds.height - 250.f
    );

    // configurar fuente compartida para torres
    Tower::setSharedFont(game->getFont());

    // configurar sistema de audio para torres
    Tower::setAudioSystem(&game->getAudioSystem());

    // configurar fuente compartida para enemigos
    Enemy::setSharedFont(game->getFont());

    // configurar sistema de audio para enemigos
    Enemy::setAudioSystem(&game->getAudioSystem());

    // inicializar algoritmo genetico con parametros optimizados
    geneticsSystem = std::make_unique<Genetics>(20, 0.25f, 0.85f);

    // calcular camino inicial para establecer longitud de referencia
    auto initialPath = Pathfinding::findPath(gameGrid.get(), spawnPoint, goalPoint);

    // inicializar gestor de oleadas con parametros base
    waveManager = std::make_unique<WaveManager>(initialPath, gameGrid.get(), goalPoint, 1.5f);

    // establecer longitud del camino en ambos sistemas para calculos de fitness
    if (!initialPath.empty()) {
        float pathLength = 0.0f;
        for (size_t i = 0; i < initialPath.size() - 1; i++) {
            sf::Vector2f segment = initialPath[i+1] - initialPath[i];
            pathLength += std::sqrt(segment.x * segment.x + segment.y * segment.y);
        }
        waveManager->setPathTotalLength(pathLength);
        geneticsSystem->setPathTotalLength(pathLength);
        std::cout << "Longitud del camino establecida: " << pathLength << "\n";
    }

    // obtener cromosomas para la primera oleada
    DynamicArray<Chromosome> firstWaveChromosomes = geneticsSystem->getChromosomesForWave(1);
    waveManager->setWaveChromosomes(firstWaveChromosomes);

    // comenzar inmediatamente la primera oleada
    waveManager->startNextWave();

    // crear panel de estadisticas para mostrar progreso genetico
    statsPanel = std::make_unique<StatsPanel>(game->getFont());
}



// carga todos los efectos de sonido necesarios para el juego
void GameplayState::loadGameplaySounds() {

    // sonido cuando muere un enemigo
    if (!game->getAudioSystem().loadSound("death", "assets/audio/death.wav")) {
        std::cerr << "error: no se pudo cargar death.mp3" << std::endl;
    }

    // sonido al colocar una torre
    if (!game->getAudioSystem().loadSound("colocarTorre", "assets/audio/colocarTorre.wav")) {
        std::cerr << "error: no se pudo cargar colocarTorre.mp3" << std::endl;
    }

    // sonido al mejorar una torre
    if (!game->getAudioSystem().loadSound("upgrade", "assets/audio/upgrade.mp3")) {
        std::cerr << "error: no se pudo cargar upgrade.mp3" << std::endl;
    }

    // sonidos de ataques de cada tipo de torre
    if (!game->getAudioSystem().loadSound("arrow", "assets/audio/flecha.mp3")) {
        std::cerr << "error: no se pudo cargar flecha.mp3" << std::endl;
    }
    if (!game->getAudioSystem().loadSound("fireball", "assets/audio/fireball.mp3")) {
        std::cerr << "error: no se pudo cargar fireball.mp3" << std::endl;
    }
    if (!game->getAudioSystem().loadSound("cannonball", "assets/audio/roca.mp3")) {
        std::cerr << "error: no se pudo cargar roca.mp3" << std::endl;
    }
}



// inicia la musica de fondo del gameplay
void GameplayState::startGameplayMusic() {
    std::cout << "Iniciando música de gameplay..." << std::endl;

    if (!game->getAudioSystem().playMusicIfNotPlaying("assets/audio/gameplay-theme.mp3", true)) {
        std::cerr << "error: no se pudo cargar gameplay-theme.mp3" << std::endl;
    }
}



// detiene temporalmente la musica de fondo
void GameplayState::pauseMusic() {
    if (!musicPaused) {
        game->getAudioSystem().stopAllMusic();
        musicPaused = true;
    }
}



// reanuda la musica de fondo si estaba pausada
void GameplayState::resumeMusic() {
    if (musicPaused) {
        startGameplayMusic();
        musicPaused = false;
        std::cout << "Música reanudada" << std::endl;
    }
}



// detiene completamente la musica de fondo
void GameplayState::stopMusic() {
    game->getAudioSystem().stopAllMusic();
    musicPaused = false;
    std::cout << "Música detenida" << std::endl;
}



// carga y configura la imagen de fondo del nivel
bool GameplayState::loadBackgroundTexture() {
    auto& window = game->getWindow();
    greenBackground.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    greenBackground.setFillColor(sf::Color(34, 139, 34)); // verde bosque como respaldo

    if (!backgroundTexture.loadFromFile("assets/images/backgrounds/backgroundGame.png")) {
        std::cerr << "error: No se pudo cargar gameplay_background.png" << std::endl;
        backgroundLoaded = false;
        return false;
    }

    // ajustar sprite de fondo para cubrir toda la pantalla
    backgroundSprite.setTexture(backgroundTexture);

    // calcular escala necesaria para llenar la ventana
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);

    backgroundLoaded = true;
    return true;
}



// define donde aparecen los enemigos y hacia donde se dirigen
void GameplayState::initializeSpawnAndGoalPoints() {
    float gridStartX = gameGrid->getX();
    float gridStartY = gameGrid->getY();
    int rows = gameGrid->getRows();
    int cols = gameGrid->getCols();
    float cellSize = gameGrid->getCellSize();

    // usar fila central para el camino principal
    int middleRow = rows / 2;

    // punto de aparicion en el borde izquierdo
    spawnPoint = sf::Vector2f(gridStartX - cellSize/2, gridStartY + middleRow * cellSize + cellSize/2);

    // punto objetivo en el borde derecho
    goalPoint = sf::Vector2f(gridStartX + (cols + 1) * cellSize + cellSize / 2, gridStartY + middleRow * cellSize + cellSize / 2);
}



// verifica si se puede colocar una torre sin bloquear caminos
bool GameplayState::canPlaceTowerAt(Cell* cell) {
    if (!cell || cell->hasTower()) {
        return false;
    }

    // simular colocacion temporal para verificar impacto
    cell->placeTower(std::make_shared<Archer>());

    // verificar que sigue existiendo camino desde spawn hasta objetivo
    bool hasPath = Pathfinding::hasValidPath(gameGrid.get(), spawnPoint, goalPoint);

    // verificar que enemigos actuales pueden seguir llegando al objetivo
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

    // verificar que futuros enemigos tendran camino disponible
    if (hasPath && waveManager->isWaveInProgress()) {
        auto spawnPath = Pathfinding::findPath(gameGrid.get(), spawnPoint, goalPoint);
        if (spawnPath.empty()) {
            hasPath = false;
        }
    }

    // remover torre temporal y devolver resultado
    cell->placeTower(nullptr);
    return hasPath;
}



// actualiza los caminos de todos los enemigos vivos
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



// procesa entradas del jugador y controla interacciones
void GameplayState::handleEvents(sf::Event& event) {
    // si el juego termino, solo permitir salir
    if (gameOver) {
        if (event.type == sf::Event::KeyPressed) {
            auto menuState = std::make_shared<MenuState>();
            game->pushState(menuState);
        }
        return;
    }

    // manejar clics en botones de torres si existen
    for (auto& button : towerButtons) {
        if (button->handleEvent(event)) {
            return; // salir si se hizo clic en un boton
        }
    }

    // cancelar seleccion si se hace clic fuera de botones y celda activa
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
        // pausar juego con escape
        if (event.key.code == sf::Keyboard::Escape) {
            auto pauseState = std::make_shared<PauseState>();
            game->pushState(pauseState);
            recalculateEnemyPaths();
        }
        // forzar siguiente oleada con n (para pruebas)
        else if (event.key.code == sf::Keyboard::N && !waveManager->isWaveInProgress()) {
            prepareNextGeneration();
        }
    }

    // manejar movimiento del mouse sobre la cuadricula
    if (event.type == sf::Event::MouseMoved && gameGrid) {
        float mouseX = static_cast<float>(event.mouseMove.x);
        float mouseY = static_cast<float>(event.mouseMove.y);

        gameGrid->clearSelection();

        // evitar resaltar celdas si hay botones activos
        if (towerButtons.empty()) {
            Cell* hoveredCell = gameGrid->getCellAtPosition(mouseX, mouseY);
            if (hoveredCell) {
                hoveredCell->setSelected(true);
            }
        }
    }

    // evitar seleccionar otra celda si ya hay una esperando
    if (selectedCellForPlacement && !towerButtons.empty()) {
        return;
    }

    // manejar clic izquierdo para mostrar opciones de torres
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

            // crear boton para torre arquero
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

            // aplicar texturas de madera a los botones
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

            // crear boton para torre mago
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

            // aplicar texturas de madera
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

            // crear boton para torre artillero
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

            // aplicar texturas de madera
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

                // aplicar texturas de madera al boton de mejora
                btnUpgrade->setTextures(
                    "assets/images/buttons/wood_button_normal.png",
                    "assets/images/buttons/wood_button_selected.png"
                );

                towerButtons.push_back(btnUpgrade);

                upgradeGoldText.setFont(game->getFont());
                upgradeGoldText.setCharacterSize(16);
                upgradeGoldText.setFillColor(sf::Color::White);
                upgradeGoldText.setString(std::to_string(tower->getUpgradeCost()) + "g");

                // centrar texto debajo del boton
                sf::Vector2f btnPos((basePos.x - btnSize.x) + 235.0f / 2.f, basePos.y - 85.f);
                sf::FloatRect textBounds = upgradeGoldText.getLocalBounds();

                upgradeGoldText.setPosition(
                    btnPos.x + (btnSize.x / 2.f) - (textBounds.width / 2.f),
                    btnPos.y + btnSize.y + 8.f
                );

                showUpgradeGoldText = true;
            }
        }
    }
}



// actualiza la logica principal del juego cada frame
void GameplayState::update(float dt) {

    // reiniciar musica si se detuvo inesperadamente
    if (!musicPaused && !game->getAudioSystem().isMusicPlaying()) {
        std::cout << "La música se detuvo inesperadamente, reiniciando..." << std::endl;
        startGameplayMusic();
    }

    // detener actualizaciones si el juego termino
    if (gameOver) {
        return;
    }

    // obtener nuevos enemigos del gestor de oleadas
    auto newEnemies = waveManager->update(dt);

    // mostrar debug cuando aparecen enemigos
    if (!newEnemies.empty()) {
        std::cout << "=== APARECIERON " << newEnemies.size() << " NUEVOS ENEMIGOS ===\n";
    }

    // agregar nuevos enemigos con sus caminos calculados
    for (auto& enemy : newEnemies) {
        // calcular ruta optima para el enemigo
        auto path = Pathfinding::findPath(gameGrid.get(), enemy->getPosition(), goalPoint);
        if (!path.empty()) {
            enemy->setPath(path);
        }
        enemies.push_back(std::move(enemy));
    }

    // mostrar informacion de debug periodicamente
    static float debugTimer = 0;
    debugTimer += dt;
    if (debugTimer >= 2.0f) { // cada 2 segundos
        std::cout << "Enemigos activos: " << enemies.size()
                  << ", Oleada en progreso: " << (waveManager->isWaveInProgress() ? "SÍ" : "NO")
                  << ", Enemigos spawneados: " << waveManager->getEnemiesSpawned()
                  << ", Generación: " << geneticsSystem->getGeneration() << "\n";
        debugTimer = 0;
    }

    // actualizar estados individuales de enemigos
    updateEnemyStates(dt);

    // procesar ataques de todas las torres
    handleTowerAttacks(dt);

    // verificar si es momento de preparar siguiente generacion
    if (!gameOver) {
        bool shouldPrepareNext = enemies.empty() && !waveManager->isWaveInProgress() && waveManager->getEnemiesSpawned() > 0;

        if (shouldPrepareNext) {
            std::cout << "=== PREPARANDO SIGUIENTE GENERACIÓN ===\n";
            std::cout << "Enemigos muertos: " << enemiesKilled << "\n";

            prepareNextGeneration(); // calcular fitness y crear nueva generacion

            // actualizar panel con nuevos datos
            updateStatsPanel();
        } else {
            // actualizar panel durante la oleada
            updateStatsPanel();
        }

    } else {
        // actualizar panel una vez mas al terminar el juego
        updateStatsPanel();
    }
}



// actualiza el estado de todos los enemigos activos
void GameplayState::updateEnemyStates(float dt) {
    for (auto it = enemies.begin(); it != enemies.end();) {
        (*it)->update(dt);

        // verificar estado del enemigo
        if (!(*it)->isAlive()) {
            // procesar muerte y obtener recompensas
            processEnemyDeath(*it);

            // remover enemigo de la lista
            size_t index = it - enemies.begin();
            enemies.erase(index);
            it = enemies.begin() + index;
        }
        else if ((*it)->hasReachedEnd()) {
            // enemigo llego al final - terminar juego
            processEnemyReachedEnd(*it);
            ++it;
        }
        else {
            ++it;
        }
    }
}



// procesa la muerte de un enemigo y actualiza estadisticas
void GameplayState::processEnemyDeath(std::unique_ptr<Enemy>& enemy) {
    int id = enemy->getId();

    // mostrar informacion de debug sobre el enemigo muerto
    std::cout << "=== ENEMIGO " << id << " MURIÓ ===\n";
    std::cout << "Distancia: " << enemy->getTotalDistanceTraveled()
              << ", Daño recibido: " << enemy->getTotalDamageReceived()
              << ", Tiempo: " << enemy->getTimeAlive() << "\n";

    // detectar si inicio una nueva oleada y reiniciar contadores
    int currentWave = waveManager->getCurrentWave();
    if (currentWave != currentWaveForStats) {
        std::cout << "=== NUEVA OLEADA DETECTADA: " << currentWave << " (anterior: " << currentWaveForStats << ") ===\n";
        enemiesKilledThisWave = 0;
        currentWaveFitnessList.clear();
        currentWaveForStats = currentWave;
        statsPanel->resetForNewWave();
        std::cout << "Panel de estadísticas reiniciado para nueva oleada\n";
    }

    // registrar rendimiento en el gestor de oleadas
    waveManager->trackEnemyDeath(id, *enemy);

    // reproducir sonido de muerte
    game->getAudioSystem().playSound("death");

    // otorgar recompensa de oro
    playerGold += enemy->getGoldReward();
    std::cout << "Gold + " << enemy->getGoldReward() << " (Total: " << playerGold << ")\n";

    // actualizar contadores de enemigos eliminados
    enemiesKilled++;
    enemiesKilledThisWave++;

    // calcular fitness del enemigo muerto para estadisticas
    Chromosome tempChromosome;
    tempChromosome.calculateFitness(false, enemy->getTotalDistanceTraveled(),
                                   enemy->getTotalDamageReceived(), enemy->getTimeAlive(),
                                   1000.0f); // usar longitud del camino actual

    currentWaveFitnessList.push_back(tempChromosome.getFitness());

    std::cout << "Fitness del enemigo muerto: " << tempChromosome.getFitness() << "\n";
}



// procesa cuando un enemigo llega al final y termina el juego
void GameplayState::processEnemyReachedEnd(std::unique_ptr<Enemy>& enemy) {
    int id = enemy->getId();

    std::cout << "=== ENEMIGO " << id << " LLEGÓ AL FINAL - GAME OVER ===\n";
    std::cout << "Estadísticas finales - Distancia: " << enemy->getTotalDistanceTraveled()
              << ", Daño recibido: " << enemy->getTotalDamageReceived()
              << ", Tiempo vivo: " << enemy->getTimeAlive() << "s\n";

    // registrar rendimiento antes de terminar el juego
    waveManager->trackEnemyReachedEnd(id, *enemy);

    gameOver = true;
}



// actualiza la informacion mostrada en el panel de estadisticas
void GameplayState::updateStatsPanel() {
    statsPanel->update(
        geneticsSystem->getGeneration(),
        enemiesKilledThisWave,
        currentWaveFitnessList,
        geneticsSystem->getMutationRate(),
        geneticsSystem->getMutationCount()
    );

    if (gameOver) {
        statsPanel->setVisible(false);
    }
}



// recolecta datos de rendimiento de enemigos que siguen vivos
void GameplayState::collectEnemyPerformanceData() {
    std::cout << "=== RECOLECTANDO DATOS DE ENEMIGOS ACTIVOS ===\n";

    // actualizar informacion de enemigos que aun estan luchando
    for (const auto& enemy : enemies) {
        if (enemy->isAlive()) {
            int id = enemy->getId();
            std::cout << "Actualizando datos de enemigo activo " << id << "\n";

            // registrar progreso actual de enemigos vivos
            waveManager->trackEnemyPerformance(id, false,
                                             enemy->getTotalDistanceTraveled(),
                                             enemy->getTotalDamageReceived(),
                                             enemy->getTimeAlive());
        }
    }
}



// genera la siguiente generacion usando algoritmo genetico
void GameplayState::prepareNextGeneration() {
    std::cout << "=== INICIO prepareNextGeneration() ===\n";

    // recopilar datos finales de enemigos activos
    collectEnemyPerformanceData();

    // obtener metricas de rendimiento de la oleada completa
    DynamicArray<bool> reachedEnd = waveManager->getEnemiesReachedEnd();
    DynamicArray<float> distancesTraveled = waveManager->getDistancesTraveled();
    DynamicArray<float> damagesReceived = waveManager->getDamagesReceived();
    DynamicArray<float> timesAlive = waveManager->getTimesAlive();

    std::cout << "Datos de performance recopilados:\n";
    std::cout << "- Enemigos que llegaron al final: " << reachedEnd.size() << "\n";
    std::cout << "- Distancias: " << distancesTraveled.size() << "\n";
    std::cout << "- Daños recibidos: " << damagesReceived.size() << "\n";
    std::cout << "- Tiempos vivos: " << timesAlive.size() << "\n";

    // calcular estadisticas de la oleada para debug
    if (!damagesReceived.empty()) {
        float avgDamage = 0.0f;
        float maxDamage = 0.0f;
        for (float damage : damagesReceived) {
            avgDamage += damage;
            maxDamage = std::max(maxDamage, damage);
        }
        avgDamage /= damagesReceived.size();

        std::cout << "Estadísticas de daño - Promedio: " << avgDamage
                  << ", Máximo: " << maxDamage << "\n";
    }

    std::cout << "Evaluando población con datos reales...\n";
    // evaluar poblacion con datos reales de rendimiento
    geneticsSystem->evaluatePopulation(reachedEnd, distancesTraveled, damagesReceived, timesAlive);

    // mostrar fitness calculados para debug
    std::cout << "=== FITNESS DESPUÉS DE EVALUAR ===\n";
    DynamicArray<float> fitnessScores = geneticsSystem->getCurrentFitnessScores();
    float totalFitness = 0.0f;
    float maxFitness = 0.0f;
    for (size_t i = 0; i < fitnessScores.size() && i < 5; ++i) {
        std::cout << "Cromosoma " << i << ": " << fitnessScores[i] << "\n";
        totalFitness += fitnessScores[i];
        maxFitness = std::max(maxFitness, fitnessScores[i]);
    }

    if (!fitnessScores.empty()) {
        float avgFitness = totalFitness / std::min(static_cast<size_t>(5), fitnessScores.size());
        std::cout << "Fitness promedio (top 5): " << avgFitness
                  << ", Fitness máximo: " << maxFitness << "\n";
    }

    std::cout << "Creando siguiente generación...\n";
    // crear nueva generacion mediante seleccion, cruzamiento y mutacion
    geneticsSystem->createNextGeneration();

    // seleccionar mejores cromosomas para la siguiente oleada
    int nextWaveSize = waveManager->getEnemiesPerWave();
    DynamicArray<Chromosome> nextWaveChromosomes = geneticsSystem->getChromosomesForWave(nextWaveSize);

    std::cout << "Cromosomas seleccionados para oleada: " << nextWaveChromosomes.size()
              << " (necesarios: " << nextWaveSize << ")\n";

    // configurar gestor de oleadas con nuevos cromosomas
    waveManager->setWaveChromosomes(nextWaveChromosomes);

    // comenzar la siguiente oleada
    waveManager->startNextWave();

    std::cout << "=== FIN prepareNextGeneration() ===\n";
}



// dibuja todos los elementos visuales del juego en pantalla
void GameplayState::render(sf::RenderWindow& window) {

    // dibujar fondo verde como base
    window.draw(greenBackground);

    // dibujar cuadricula del juego
    if (gameGrid) {
        gameGrid->draw(window);
    }

    // dibujar imagen de fondo decorativa
    window.draw(backgroundSprite);

    // dibujar proyectiles de todas las torres
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

    // dibujar todos los enemigos activos
    for (const auto& enemy : enemies) {
        window.draw(*enemy);
    }

    // dibujar botones de seleccion de torres
    for (const auto& button : towerButtons) {
        button->draw(window);
    }

    // dibujar precios de las torres
    for (const auto& text : towerPriceTexts) {
        window.draw(*text);
    }

    // mostrar pantalla de game over si es necesario
    if (gameOver) {
        auto& font = game->getFont();
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER");

        // centrar texto principal
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setPosition(
            (window.getSize().x - textBounds.width) / 2.0f,
            window.getSize().y * 0.4f
        );

        // texto de instrucciones
        sf::Text instructionText;
        instructionText.setFont(font);
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setString("press any key");

        // centrar texto de instrucciones
        textBounds = instructionText.getLocalBounds();
        instructionText.setPosition(
            (window.getSize().x - textBounds.width) / 2.0f,
            window.getSize().y * 0.5f
        );

        window.draw(gameOverText);
        window.draw(instructionText);
    }

    // mostrar advertencia de oro insuficiente con efecto parpadeante
    if (showGoldWarning) {
        float t = goldWarningClock.getElapsedTime().asSeconds();

        // mostrar durante 2 segundos
        if (t < 2.0f) {
            // efecto de parpadeo
            if (static_cast<int>(t * 5) % 2 == 0) {
                window.draw(insufficientGoldText);
            }
        } else {
            showGoldWarning = false;
        }
    }

    // mostrar advertencia de bloqueo de camino con efecto parpadeante
    if (showPathBlocked) {
        float t = pathBlockedClock.getElapsedTime().asSeconds();

        // mostrar durante 2 segundos
        if (t < 2.0f) {
            // efecto de parpadeo
            if (static_cast<int>(t * 5) % 2 == 0) {
                window.draw(pathBlockedText);
            }
        } else {
            showPathBlocked = false;
        }
    }

    // mostrar costo de mejora de torre si es necesario
    if (showUpgradeGoldText) {
        window.draw(upgradeGoldText);
    }

    // mostrar cantidad de oro actual del jugador
    sf::Text goldText;
    goldText.setFont(game->getFont());
    goldText.setCharacterSize(40);
    goldText.setFillColor(sf::Color(255, 215, 0));

    goldText.setString("GOLD: " + std::to_string(playerGold));
    goldText.setPosition(1250.f, 950.f);

    // dibujar panel de estadisticas geneticas
    if (statsPanel) {
        statsPanel->draw(window);
    }

    window.draw(goldText);
}



// libera todos los recursos al salir del estado
void GameplayState::cleanup() {
    stopMusic();
    gameGrid.reset();
    enemies.clear();
    waveManager.reset();
    geneticsSystem.reset();
}



// procesa los ataques de todas las torres hacia enemigos en rango
void GameplayState::handleTowerAttacks(float dt) {
    const auto& cellGrid = gameGrid->getCells();

    for (const auto& row : cellGrid) {
        for (const auto& cell : row) {
            if (cell.hasTower()) {
                auto tower = cell.getTower();

                // actualizar proyectiles de cada tipo de torre
                if (tower->type() == "Archer") {
                    std::dynamic_pointer_cast<Archer>(tower)->updateProjectiles(dt);
                }
                if (tower->type() == "Mage") {
                    std::dynamic_pointer_cast<Mage>(tower)->updateProjectiles(dt);
                }
                if (tower->type() == "Gunner") {
                    std::dynamic_pointer_cast<Gunner>(tower)->updateProjectiles(dt);
                }

                // buscar enemigos en rango de ataque
                for (const auto& enemyPtr : enemies) {
                    if (enemyPtr->isAlive()) {
                        float dx = enemyPtr->getPosition().x - cell.getPosition().x;
                        float dy = enemyPtr->getPosition().y - cell.getPosition().y;
                        float distance = std::hypot(dx, dy);

                        if (distance <= tower->getRange()) {
                            tower->attack(*enemyPtr, enemies);
                            break; // solo un ataque por ciclo
                        }
                    }
                }
            }
        }
    }
}



// verifica si el clic fue fuera de botones y celda seleccionada
bool GameplayState::clickedOutsideButtonsAndSelectedCell(const sf::Vector2f& mousePos) const {
    if (!selectedCellForPlacement) return false;

    // verificar si se hizo clic dentro de algun boton
    for (const auto& button : towerButtons) {
        if (button->getBounds().contains(mousePos)) {
            return false;
        }
    }

    // verificar si se hizo clic dentro de la celda seleccionada
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