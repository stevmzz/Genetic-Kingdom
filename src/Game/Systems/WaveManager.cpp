#include "../include/Game/Systems/WaveManager.h"
#include "../include/Game/Enemies/Ogre.h"
#include <random>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>

// constructor
WaveManager::WaveManager(const std::vector<sf::Vector2f>& path, float enemySpawnInterval)
    :   currentWave(0),
        enemyPath(path),
        enemySpawnInterval(enemySpawnInterval),
        waveInProgress(false),
        enemiesSpawned(0),
        enemiesRemaining(0),
        enemiesPerWave(1), // comenzar con un enemigo
        pathTotalLength(0.0f),
        lastWaveMaxProgress(0.0f),
        lastWaveAvgProgress(0.0f) {

    // establecer la posicion de generacion como el primer punto del camino
    if (!path.empty()) {
        spawnPosition = path[0];
    } else {
        spawnPosition = sf::Vector2f(0, 0);
    }

    // calcular la longitud total del camino
    calculatePathLength();

    // inicializar el temporizador para generacion de enemigos
    enemySpawnTimer.restart();
}



// calcular la longitud total del camino
void WaveManager::calculatePathLength() {
    pathTotalLength = 0.0f;
    if (enemyPath.size() > 1) {
        for (size_t i = 0; i < enemyPath.size() - 1; i++) {
            sf::Vector2f segment = enemyPath[i+1] - enemyPath[i];
            pathTotalLength += std::sqrt(segment.x * segment.x + segment.y * segment.y);
        }
    }
}



// actualizar el estado del gestor de oleadas
std::vector<std::unique_ptr<Enemy>> WaveManager::update(float dt) {
    std::vector<std::unique_ptr<Enemy>> newEnemies;

    // si hay una oleada en curso, generar enemigos segun el intervalo
    if (waveInProgress && enemiesRemaining > 0) {
        if (enemySpawnTimer.getElapsedTime().asSeconds() >= enemySpawnInterval) {
            // obtener el cromosoma para este enemigo
            Chromosome chromosome;

            if (!currentWaveChromosomes.empty()) {
                // si hay suficientes cromosomas, usar uno existente
                if (enemiesSpawned < currentWaveChromosomes.size()) {
                    chromosome = currentWaveChromosomes[enemiesSpawned];

                } else {
                    // de lo contrario, usar un cromosoma aleatorio de los disponibles
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<size_t> dist(0, currentWaveChromosomes.size() - 1);
                    chromosome = currentWaveChromosomes[dist(gen)];
                }

            } else {
                // si no hay cromosomas, usar uno predeterminado
                chromosome = Chromosome();
            }

            // preparar estructura para seguimiento de rendimiento
            EnemyPerformance performance;
            performance.reachedEnd = false;
            performance.distanceTraveled = 0.0f;
            performance.timeAlive = 0.0f;
            performance.chromosomeIndex = enemiesSpawned % (currentWaveChromosomes.empty() ? 1 : currentWaveChromosomes.size());
            enemyPerformanceData[enemiesSpawned] = performance;

            // crear un ogro con el cromosoma
            auto enemy = std::make_unique<Ogre>(spawnPosition, enemyPath, chromosome);
            enemy->setId(enemiesSpawned);
            newEnemies.push_back(std::move(enemy));

            // actualizar contadores
            enemiesSpawned++;
            enemiesRemaining--;

            // reiniciar el temporizador de generación
            enemySpawnTimer.restart();
        }
    }

    // verificar si la oleada ha terminado
    if (waveInProgress && enemiesRemaining == 0) {
        waveInProgress = false;
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




// Verificar si la oleada ha terminado completamente
bool WaveManager::isWaveComplete() const {
    return !waveInProgress && enemiesSpawned > 0 && enemiesRemaining == 0;
}



// iniciar manualmente una nueva oleada
void WaveManager::startNextWave() {
    currentWave++;

    // calcular la cantidad de enemigos para esta oleada (si no es la primera)
    if (currentWave > 1) {
        calculateNextWaveEnemyCount();
    }

    enemiesSpawned = 0;
    enemiesRemaining = enemiesPerWave;
    waveInProgress = true;
    enemySpawnTimer.restart();

    // limpiar datos de rendimiento de la oleada anterior
    enemyPerformanceData.clear();
}




// establecer cromosomas para la siguiente oleada
void WaveManager::setWaveChromosomes(const std::vector<Chromosome>& chromosomes) {
    currentWaveChromosomes = chromosomes;
}




// seguimiento del rendimiento de un enemigo
void WaveManager::trackEnemyPerformance(int enemyId, bool reachedEnd, float distanceTraveled, float timeAlive) {
    if (enemyPerformanceData.find(enemyId) != enemyPerformanceData.end()) {
        enemyPerformanceData[enemyId].reachedEnd = reachedEnd;
        enemyPerformanceData[enemyId].distanceTraveled = distanceTraveled;
        enemyPerformanceData[enemyId].timeAlive = timeAlive;
    }
}




// calcular la cantidad de enemigos para la siguiente oleada
void WaveManager::calculateNextWaveEnemyCount() {
    // recopilar datos sobre el progreso de los enemigos
    std::vector<float> progressValues;
    float maxProgress = 0.0f;
    float totalProgress = 0.0f;

    for (const auto& [id, data] : enemyPerformanceData) {
        float progress = 0.0f;
        if (pathTotalLength > 0.0f) {
            progress = std::min(data.distanceTraveled / pathTotalLength, 1.0f);
        }

        progressValues.push_back(progress);
        maxProgress = std::max(maxProgress, progress);
        totalProgress += progress;
    }

    float avgProgress = progressValues.empty() ? 0.0f : totalProgress / progressValues.size();

    // guardar las estadisticas de esta oleada
    lastWaveMaxProgress = maxProgress;
    lastWaveAvgProgress = avgProgress;

    if (maxProgress > 0.8f) {
        // si algún enemigo llegó muy lejos (>80% del camino), reducir o mantener cantidad
        if (enemiesPerWave > 1) {
            enemiesPerWave = std::max(1, enemiesPerWave - 1);
        }
    }
    else if (maxProgress < 0.3f) {
        // si ningún enemigo llegó lejos (<30% del camino), aumentar cantidad bastante
        enemiesPerWave = std::min(15, enemiesPerWave + 2);
    }
    else if (maxProgress < 0.5f) {
        // si llegaron a medio camino, aumentar cantidad ligeramente
        enemiesPerWave = std::min(15, enemiesPerWave + 1);
    }

    // entre 50-80% mantener igual cantidad

    // ajuste adicional basado en cromosomas (si todos los enemigos usaron el mismo cromosoma)
    if (currentWaveChromosomes.size() == 1) {
        Chromosome& chromosome = currentWaveChromosomes[0];

        // si el enemigo es muy rápido y tiene mucha salud, posiblemente reducir cantidad
        float speedFactor = chromosome.getSpeed() / 100.0f;
        float healthFactor = chromosome.getHealth() / 300.0f;
        float strengthFactor = (speedFactor + healthFactor) / 2.0f;

        if (strengthFactor > 0.7f && maxProgress > 0.6f) {
            // enemigo fuerte que llegó lejos: reducir cantidad
            enemiesPerWave = std::max(1, enemiesPerWave - 1);
        }

        else if (strengthFactor < 0.3f && maxProgress < 0.4f) {
            // enemigo débil que no llegó lejos: aumentar cantidad
            enemiesPerWave = std::min(15, enemiesPerWave + 1);
        }
    }

    // garantizar valores mínimos y máximos razonables (minimo y maximo de enemigos por oleada)
    enemiesPerWave = std::max(1, std::min(6, enemiesPerWave));
}



// establecer la longitud total del camino
void WaveManager::setPathTotalLength(float length) {
    pathTotalLength = length;
}



// obtener datos para evaluación genética:
std::vector<bool> WaveManager::getEnemiesReachedEnd() const {
    std::vector<bool> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.reachedEnd);

    }
    return result;
}

std::vector<float> WaveManager::getDistancesTraveled() const {
    std::vector<float> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.distanceTraveled);
    }
    return result;
}

std::vector<float> WaveManager::getTimesAlive() const {
    std::vector<float> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.timeAlive);
    }
    return result;
}



// restablecer el gestor a su estado inicial
void WaveManager::reset() {
    currentWave = 0;
    waveInProgress = false;
    enemiesSpawned = 0;
    enemiesRemaining = 0;
    enemiesPerWave = 1;
    enemyPerformanceData.clear();
    lastWaveMaxProgress = 0.0f;
    lastWaveAvgProgress = 0.0f;
    enemySpawnTimer.restart();
}