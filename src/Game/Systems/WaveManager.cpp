#include "../include/Game/Systems/WaveManager.h"
#include "../include/Game/Enemies/Ogre.h"
#include "../include/Game/Enemies/DarkElves.h"
#include "../include/Game/Enemies/Harpy.h"
#include "../include/Game/Enemies/Mercenary.h"
#include "../include/Game/Systems/Pathfinding.h"
#include <random>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>

// inicializa el gestor de oleadas con parametros basicos
WaveManager::WaveManager(const DynamicArray<sf::Vector2f>& path, Grid* grid, const sf::Vector2f& goal, float enemySpawnInterval)
    :   currentWave(0),
        enemyPath(path),
        gridReference(grid),
        goalPoint(goal),
        enemySpawnInterval(enemySpawnInterval),
        waveInProgress(false),
        enemiesSpawned(0),
        enemiesRemaining(0),
        enemiesPerWave(1), // empezar con un solo enemigo
        pathTotalLength(0.0f),
        lastWaveMaxProgress(0.0f),
        lastWaveAvgProgress(0.0f),
        lastWaveAvgDamage(0.0f),
        lastWaveEffectiveness(0.0f),
        lastWaveReachedCount(0) {

    // configurar punto de aparicion en el inicio del camino
    if (!path.empty()) {
        spawnPosition = path[0];
    } else {
        spawnPosition = sf::Vector2f(0, 0);
    }

    // medir distancia total del recorrido para calculos posteriores
    calculatePathLength();

    // preparar temporizador para controlar aparicion de enemigos
    enemySpawnTimer.restart();
}



// calcula la distancia total que deben recorrer los enemigos
void WaveManager::calculatePathLength() {
    pathTotalLength = 0.0f;
    if (enemyPath.size() > 1) {
        for (size_t i = 0; i < enemyPath.size() - 1; i++) {
            sf::Vector2f segment = enemyPath[i+1] - enemyPath[i];
            pathTotalLength += std::sqrt(segment.x * segment.x + segment.y * segment.y);
        }
    }

    std::cout << "Longitud total del camino calculada: " << pathTotalLength << "\n";
}



// genera enemigos durante una oleada activa segun intervalos configurados
DynamicArray<std::unique_ptr<Enemy>> WaveManager::update(float dt) {
    DynamicArray<std::unique_ptr<Enemy>> newEnemies;

    // verificar si es momento de generar un nuevo enemigo
    if (waveInProgress && enemiesRemaining > 0) {
        if (enemySpawnTimer.getElapsedTime().asSeconds() >= enemySpawnInterval) {
            // seleccionar cromosoma para definir caracteristicas del enemigo
            Chromosome chromosome;

            if (!currentWaveChromosomes.empty()) {
                // usar cromosoma especifico si esta disponible
                if (enemiesSpawned < currentWaveChromosomes.size()) {
                    chromosome = currentWaveChromosomes[enemiesSpawned];

                } else {
                    // seleccionar cromosoma aleatorio de los disponibles
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<size_t> dist(0, currentWaveChromosomes.size() - 1);
                    chromosome = currentWaveChromosomes[dist(gen)];
                }

            } else {
                // usar cromosoma por defecto si no hay disponibles
                chromosome = Chromosome();
            }

            // inicializar estructura para seguimiento de rendimiento
            EnemyPerformance performance;
            performance.reachedEnd = false;
            performance.distanceTraveled = 0.0f;
            performance.damageReceived = 0.0f;
            performance.timeAlive = 0.0f;
            performance.damageEffectiveness = 0.0f;
            performance.chromosomeIndex = enemiesSpawned % (currentWaveChromosomes.empty() ? 1 : currentWaveChromosomes.size());
            enemyPerformanceData[enemiesSpawned] = performance;

            // crear tipo de enemigo segun rotacion basada en oleada actual
            std::unique_ptr<Enemy> enemy;
            int enemyType = (currentWave - 1) % 4;  // alternar entre 4 tipos diferentes

            switch(enemyType) {
                case 0:
                    enemy = std::make_unique<Ogre>(spawnPosition, enemyPath, chromosome, currentWave);
                break;
                case 1:
                    enemy = std::make_unique<DarkElves>(spawnPosition, enemyPath, chromosome, currentWave);
                break;
                case 2:
                    enemy = std::make_unique<Harpy>(spawnPosition, enemyPath, chromosome, currentWave);
                break;
                case 3:
                    enemy = std::make_unique<Mercenary>(spawnPosition, enemyPath, chromosome, currentWave);
                break;
                default:
                    enemy = std::make_unique<Ogre>(spawnPosition, enemyPath, chromosome, currentWave);
            }

            enemy->setId(enemiesSpawned);

            // calcular ruta optima usando algoritmo a* si la cuadricula esta disponible
            if (gridReference && enemy) {
                auto initialPath = Pathfinding::findPath(gridReference, spawnPosition, goalPoint);
                if (!initialPath.empty()) {
                    enemy->setPath(initialPath);
                }
            }

            newEnemies.push_back(std::move(enemy));

            // actualizar contadores de la oleada
            enemiesSpawned++;
            enemiesRemaining--;

            // resetear temporizador para el siguiente enemigo
            enemySpawnTimer.restart();
        }
    }

    // marcar oleada como completada cuando no quedan enemigos por generar
    if (waveInProgress && enemiesRemaining == 0) {
        waveInProgress = false;
        std::cout << "Oleada " << currentWave << " completada. Enemigos spawneados: " << enemiesSpawned << "\n";
    }

    return newEnemies;
}



// obtiene el numero de la oleada que se esta ejecutando actualmente
int WaveManager::getCurrentWave() const {
    return currentWave;
}



// obtiene cuantos enemigos faltan por aparecer en la oleada actual
int WaveManager::getEnemiesRemaining() const {
    return enemiesRemaining;
}



// verifica si hay una oleada ejecutandose en este momento
bool WaveManager::isWaveInProgress() const {
    return waveInProgress;
}



// verifica si la oleada termino completamente de generar enemigos
bool WaveManager::isWaveComplete() const {
    return !waveInProgress && enemiesSpawned > 0 && enemiesRemaining == 0;
}



// inicia una nueva oleada y analiza el rendimiento de la anterior
void WaveManager::startNextWave() {
    currentWave++;

    // procesar datos de rendimiento si no es la primera oleada
    if (currentWave > 1) {
        analyzeWavePerformance();
        calculateNextWaveEnemyCount();
    }

    enemiesSpawned = 0;
    enemiesRemaining = enemiesPerWave;
    waveInProgress = true;
    enemySpawnTimer.restart();

    // reiniciar sistema de seguimiento para la nueva oleada
    enemyPerformanceData.clear();

    std::cout << "=== INICIANDO OLEADA " << currentWave << " ===\n";
    std::cout << "Enemigos a generar: " << enemiesPerWave << "\n";
}



// define los cromosomas que se usaran para generar enemigos en la oleada
void WaveManager::setWaveChromosomes(const DynamicArray<Chromosome>& chromosomes) {
    currentWaveChromosomes = chromosomes;
    std::cout << "Cromosomas establecidos para oleada: " << chromosomes.size() << "\n";
}



// registra metricas de rendimiento de un enemigo especifico
void WaveManager::trackEnemyPerformance(int enemyId, bool reachedEnd, float distanceTraveled, float damageReceived, float timeAlive) {
    if (enemyPerformanceData.find(enemyId) != enemyPerformanceData.end()) {
        enemyPerformanceData[enemyId].reachedEnd = reachedEnd;
        enemyPerformanceData[enemyId].distanceTraveled = distanceTraveled;
        enemyPerformanceData[enemyId].damageReceived = damageReceived;
        enemyPerformanceData[enemyId].timeAlive = timeAlive;

        // calcular que tan efectivo fue el daño recibido vs distancia recorrida
        if (distanceTraveled > 0) {
            enemyPerformanceData[enemyId].damageEffectiveness = damageReceived / (distanceTraveled / pathTotalLength);
        }

        std::cout << "Performance actualizada - Enemigo " << enemyId
                  << ": distancia=" << distanceTraveled
                  << ", daño=" << damageReceived
                  << ", tiempo=" << timeAlive << "\n";
    }
}



// registra datos cuando un enemigo muere en combate
void WaveManager::trackEnemyDeath(int enemyId, const Enemy& enemy) {
    trackEnemyPerformance(enemyId, false, enemy.getTotalDistanceTraveled(),
                         enemy.getTotalDamageReceived(), enemy.getTimeAlive());

    std::cout << "Enemigo " << enemyId << " murió - Efectividad: "
              << enemy.getDamageEffectiveness() << "\n";
}



// registra datos cuando un enemigo logra llegar al objetivo final
void WaveManager::trackEnemyReachedEnd(int enemyId, const Enemy& enemy) {
    trackEnemyPerformance(enemyId, true, enemy.getTotalDistanceTraveled(),
                         enemy.getTotalDamageReceived(), enemy.getTimeAlive());

    std::cout << "¡Enemigo " << enemyId << " llegó al final! Daño total recibido: "
              << enemy.getTotalDamageReceived() << "\n";
}



// analiza el rendimiento de todos los enemigos de la oleada anterior
void WaveManager::analyzeWavePerformance() {
    if (enemyPerformanceData.empty()) {
        std::cout << "No hay datos de performance para analizar\n";
        return;
    }

    std::cout << "=== ANÁLISIS DE OLEADA " << currentWave << " ===\n";

    // recopilar todas las metricas en estructuras separadas
    DynamicArray<float> progressValues;
    DynamicArray<float> damageValues;
    DynamicArray<float> effectivenessValues;

    float totalProgress = 0.0f;
    float totalDamage = 0.0f;
    float maxProgress = 0.0f;
    int reachedEndCount = 0;

    // procesar datos de cada enemigo
    for (const auto& [id, data] : enemyPerformanceData) {
        float progress = 0.0f;
        if (pathTotalLength > 0.0f) {
            progress = std::min(data.distanceTraveled / pathTotalLength, 1.0f);
        }

        progressValues.push_back(progress);
        damageValues.push_back(data.damageReceived);
        effectivenessValues.push_back(data.damageEffectiveness);

        totalProgress += progress;
        totalDamage += data.damageReceived;
        maxProgress = std::max(maxProgress, progress);

        if (data.reachedEnd) {
            reachedEndCount++;
        }
    }

    // calcular estadisticas agregadas
    float avgProgress = totalProgress / enemyPerformanceData.size();
    float avgDamage = totalDamage / enemyPerformanceData.size();
    float avgEffectiveness = calculateAverageMetric(effectivenessValues);

    // almacenar resultados para decisiones futuras
    lastWaveMaxProgress = maxProgress;
    lastWaveAvgProgress = avgProgress;
    lastWaveAvgDamage = avgDamage;
    lastWaveEffectiveness = avgEffectiveness;
    lastWaveReachedCount = reachedEndCount;

    std::cout << "Progreso máximo: " << (maxProgress * 100) << "%\n";
    std::cout << "Progreso promedio: " << (avgProgress * 100) << "%\n";
    std::cout << "Daño promedio recibido: " << avgDamage << "\n";
    std::cout << "Efectividad promedio: " << avgEffectiveness << "\n";
    std::cout << "Enemigos que llegaron al final: " << reachedEndCount << "/" << enemyPerformanceData.size() << "\n";
    std::cout << "=== FIN ANÁLISIS ===\n";
}



// determina cuantos enemigos generar en la proxima oleada segun rendimiento
void WaveManager::calculateNextWaveEnemyCount() {
    std::cout << "=== CALCULANDO ENEMIGOS PARA PRÓXIMA OLEADA ===\n";

    // usar metricas ya calculadas del analisis anterior
    float maxProgress = lastWaveMaxProgress;
    float avgProgress = lastWaveAvgProgress;
    float avgDamage = lastWaveAvgDamage;
    int reachedCount = lastWaveReachedCount;

    int oldCount = enemiesPerWave;

    // logica principal basada en que tan lejos llegaron los enemigos
    if (maxProgress > 0.8f || reachedCount > 0) {
        // enemigos muy exitosos: reducir cantidad para balancear dificultad
        if (reachedCount > enemiesPerWave * 0.3f) { // mas del 30% llego al final
            enemiesPerWave = std::max(1, enemiesPerWave - 2);
        } else if (maxProgress > 0.9f) {
            enemiesPerWave = std::max(1, enemiesPerWave - 1);
        }
        // si llegaron lejos pero no tanto, mantener cantidad actual
    }
    else if (maxProgress < 0.3f && avgDamage < 50.0f) {
        // enemigos muy debiles: aumentar cantidad significativamente
        enemiesPerWave = std::min(12, enemiesPerWave + 3);
    }
    else if (maxProgress < 0.5f) {
        // enemigos debiles: aumentar cantidad moderadamente
        enemiesPerWave = std::min(10, enemiesPerWave + 2);
    }
    else if (avgProgress < 0.4f) {
        // rendimiento general bajo: aumentar ligeramente
        enemiesPerWave = std::min(8, enemiesPerWave + 1);
    }

    // ajustes adicionales considerando resistencia al daño
    if (lastWaveEffectiveness < 0.5f && avgProgress > 0.6f) {
        // enemigos resistentes que avanzan bien: aumentar cantidad
        enemiesPerWave = std::min(10, enemiesPerWave + 1);
    }

    // consideracion especial para diversidad genetica limitada
    if (currentWaveChromosomes.size() == 1) {
        // si solo hay un cromosoma, ajustar segun su fortaleza individual
        if (maxProgress > 0.7f) {
            enemiesPerWave = std::max(1, enemiesPerWave - 1);
        } else if (maxProgress < 0.3f) {
            enemiesPerWave = std::min(15, enemiesPerWave + 2);
        }
    }

    // mantener valores dentro de rangos razonables para jugabilidad
    enemiesPerWave = std::max(1, std::min(15, enemiesPerWave));

    std::cout << "Enemigos por oleada: " << oldCount << " -> " << enemiesPerWave;
    if (enemiesPerWave != oldCount) {
        std::cout << " (cambio: " << (enemiesPerWave - oldCount) << ")";
    }
    std::cout << "\n";
}



// modifica parametros de dificultad segun el exito de los enemigos
void WaveManager::adjustDifficultyBasedOnPerformance() {
    // ajustar velocidad de aparicion segun el rendimiento pasado
    if (lastWaveMaxProgress > 0.8f) {
        // enemigos muy exitosos: generar mas lentamente
        enemySpawnInterval = std::min(3.0f, enemySpawnInterval * 1.2f);
    } else if (lastWaveMaxProgress < 0.3f) {
        // enemigos poco exitosos: generar mas rapidamente
        enemySpawnInterval = std::max(0.8f, enemySpawnInterval * 0.9f);
    }

    std::cout << "Intervalo de spawn ajustado a: " << enemySpawnInterval << "s\n";
}



// calcula un valor numerico que representa la dificultad actual
float WaveManager::getWaveDifficulty() const {
    // combinar multiples factores para una metrica compuesta
    float difficulty = 0.0f;

    // dificultad base que aumenta por oleada
    difficulty += currentWave * 0.1f;

    // factor de cantidad de enemigos simultaneos
    difficulty += enemiesPerWave * 0.05f;

    // factor de velocidad de aparicion (mas rapido = mas dificil)
    difficulty += (2.0f - enemySpawnInterval) * 0.2f;

    // factor de rendimiento historico de enemigos
    difficulty += lastWaveMaxProgress * 0.3f;

    return std::max(0.1f, std::min(10.0f, difficulty)); // mantener entre 0.1 y 10.0
}



// establece la distancia total del recorrido para calculos de progreso
void WaveManager::setPathTotalLength(float length) {
    pathTotalLength = length;
    std::cout << "Longitud total del camino establecida: " << length << "\n";
}



// recopila que enemigos lograron completar el recorrido
DynamicArray<bool> WaveManager::getEnemiesReachedEnd() const {
    DynamicArray<bool> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.reachedEnd);
    }
    return result;
}



// recopila las distancias recorridas por todos los enemigos
DynamicArray<float> WaveManager::getDistancesTraveled() const {
    DynamicArray<float> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.distanceTraveled);
    }
    return result;
}



// recopila el daño total recibido por cada enemigo
DynamicArray<float> WaveManager::getDamagesReceived() const {
    DynamicArray<float> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.damageReceived);
    }
    return result;
}



// recopila el tiempo de supervivencia de cada enemigo
DynamicArray<float> WaveManager::getTimesAlive() const {
    DynamicArray<float> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.timeAlive);
    }
    return result;
}



// recopila la efectividad del daño recibido por cada enemigo
DynamicArray<float> WaveManager::getDamageEffectiveness() const {
    DynamicArray<float> result;
    for (const auto& [id, data] : enemyPerformanceData) {
        result.push_back(data.damageEffectiveness);
    }
    return result;
}



// calcula el promedio de un conjunto de valores numericos
float WaveManager::calculateAverageMetric(const DynamicArray<float>& values) const {
    if (values.empty()) return 0.0f;

    float sum = 0.0f;
    for (float value : values) {
        sum += value;
    }
    return sum / values.size();
}



// encuentra el valor maximo en un conjunto de datos
float WaveManager::calculateMaxMetric(const DynamicArray<float>& values) const {
    if (values.empty()) return 0.0f;

    float maxVal = values[0];
    for (float value : values) {
        maxVal = std::max(maxVal, value);
    }
    return maxVal;
}



// reinicia el gestor a su configuracion inicial
void WaveManager::reset() {
    currentWave = 0;
    waveInProgress = false;
    enemiesSpawned = 0;
    enemiesRemaining = 0;
    enemiesPerWave = 1;
    enemyPerformanceData.clear();
    lastWaveMaxProgress = 0.0f;
    lastWaveAvgProgress = 0.0f;
    lastWaveAvgDamage = 0.0f;
    lastWaveEffectiveness = 0.0f;
    lastWaveReachedCount = 0;
    enemySpawnTimer.restart();

    std::cout << "WaveManager reseteado a estado inicial\n";
}