#include "../include/Game/Systems/WaveManager.h"
#include "../include/Game/Enemies/Ogre.h"
#include <iostream>

// constructor
WaveManager::WaveManager(const std::vector<sf::Vector2f>& path,
                        int enemiesPerWave,
                        float waveInterval,
                        float enemySpawnInterval)
    : currentWave(0),
      enemyPath(path),
      enemiesPerWave(enemiesPerWave),
      waveInterval(waveInterval),
      enemySpawnInterval(enemySpawnInterval),
      waveInProgress(false),
      enemiesSpawned(0),
      enemiesRemaining(0) {

    // establecer la posicion de generacion como el primer punto del camino
    if (!path.empty()) {
        spawnPosition = path[0];
    } else {
        spawnPosition = sf::Vector2f(0, 0);
    }

    // inicializar temporizadores
    waveTimer.restart();
    enemySpawnTimer.restart();
}



// actualizar el estado del gestor de oleadas
std::vector<std::unique_ptr<Enemy>> WaveManager::update(float dt) {
    std::vector<std::unique_ptr<Enemy>> newEnemies;

    // si no hay una oleada en curso, verificar si es hora de iniciar una nueva
    if (!waveInProgress) {
        if (waveTimer.getElapsedTime().asSeconds() >= waveInterval) {
            startNextWave();
        }
    }

    // si hay una oleada en curso, generar enemigos segun el intervalo
    if (waveInProgress && enemiesRemaining > 0) {
        if (enemySpawnTimer.getElapsedTime().asSeconds() >= enemySpawnInterval) {
            // crear un nuevo ogro
            auto enemy = std::make_unique<Ogre>(spawnPosition, enemyPath);
            newEnemies.push_back(std::move(enemy));

            // actualizar contadores
            enemiesSpawned++;
            enemiesRemaining--;

            // reiniciar el temporizador de generacion
            enemySpawnTimer.restart();

        }
    }

    // verificar si la oleada ha terminado
    if (waveInProgress && enemiesRemaining == 0) {
        waveInProgress = false;
        waveTimer.restart();
    }

    return newEnemies;
}



// obtener el numero de la oleada actual
int WaveManager::getCurrentWave() const {
    return currentWave;
}



// obtener la cantidad de enemigos que faltan por generar
int WaveManager::getEnemiesRemaining() const {
    return enemiesRemaining;
}



// verificar si hay una oleada en curso
bool WaveManager::isWaveInProgress() const {
    return waveInProgress;
}



// iniciar manualmente una nueva oleada
void WaveManager::startNextWave() {
    currentWave++;
    enemiesSpawned = 0;
    enemiesRemaining = enemiesPerWave;
    waveInProgress = true;
    enemySpawnTimer.restart();
}



// restablecer el gestor a su estado inicial
void WaveManager::reset() {
    currentWave = 0;
    waveInProgress = false;
    enemiesSpawned = 0;
    enemiesRemaining = 0;
    waveTimer.restart();
    enemySpawnTimer.restart();
}