#include "../include/Game/Genetics//Genetics.h"
#include <algorithm>
#include <numeric>
#include <iostream>

// configura los parametros del algoritmo genetico
Genetics::Genetics(int populationSize, float mutationRate, float crossoverRate)
    : populationSize(populationSize), mutationRate(mutationRate), crossoverRate(crossoverRate),
      generation(0), mutationCount(0), pathTotalLength(1000.0f), averageDiversity(1.0f),
      convergenceThreshold(0.05f) {

    // configurar generador aleatorio con semilla variable
    std::random_device rd;
    randomGenerator = std::mt19937(rd());

    // crear poblacion inicial con cromosomas aleatorios
    initializePopulation();
}



// crea la poblacion inicial con cromosomas generados aleatoriamente
void Genetics::initializePopulation() {
    population.clear();

    for (int i = 0; i < populationSize; ++i) {
        population.push_back(Chromosome());
    }

    std::cout << "Población inicializada con " << populationSize << " cromosomas\n";
}



// evalua el rendimiento de todos los cromosomas en la poblacion
void Genetics::evaluatePopulation(const DynamicArray<bool> &reachedEnd, const DynamicArray<float> &distancesTraveled, const DynamicArray<float> &damagesDealt, const DynamicArray<float> &timesAlive) {

    std::cout << "=== DEBUG evaluatePopulation ===\n";
    std::cout << "reachedEnd.size(): " << reachedEnd.size() << "\n";
    std::cout << "population.size(): " << population.size() << "\n";

    // verificar consistencia en el tamano de los datos de rendimiento
    size_t dataSize = reachedEnd.size();
    if (dataSize != distancesTraveled.size() || dataSize != damagesDealt.size() || dataSize != timesAlive.size()) {
        std::cout << "ERROR: Datos de performance inconsistentes\n";
        return;
    }

    // manejar caso sin datos de rendimiento
    if (dataSize == 0) {
        std::cout << "No hay datos de performance - asignando fitness base\n";
        for (auto& chromosome : population) {
            chromosome.calculateFitness(false, pathTotalLength * 0.3f, 10.0f, 5.0f, pathTotalLength);
        }
        return;
    }

    std::cout << "Evaluando " << dataSize << " cromosomas con datos de performance...\n";

    // evaluar cromosomas de forma cíclica si hay menos datos que cromosomas
    for (size_t i = 0; i < population.size(); ++i) {
        if (i < dataSize) {
            // usar datos reales para los primeros cromosomas
            std::cout << "Evaluando cromosoma " << i
                      << " - reachedEnd: " << reachedEnd[i]
                      << ", distancia: " << distancesTraveled[i]
                      << ", daño: " << damagesDealt[i]
                      << ", tiempo: " << timesAlive[i] << "\n";

            population[i].calculateFitness(reachedEnd[i], distancesTraveled[i],
                                         damagesDealt[i], timesAlive[i], pathTotalLength);
        } else {
            // ara cromosomas sin datos, usar interpolación basada en los que sí tienen datos
            size_t sourceIndex = i % dataSize; // Usar cromosoma existente como base

            // aplicar variación aleatoria a los datos base para simular diversidad
            std::uniform_real_distribution<float> variationDist(0.8f, 1.2f);
            float variation = variationDist(randomGenerator);

            float variedDistance = distancesTraveled[sourceIndex] * variation;
            float variedDamage = damagesDealt[sourceIndex] * variation;
            float variedTime = timesAlive[sourceIndex] * variation;

            population[i].calculateFitness(reachedEnd[sourceIndex], variedDistance,
                                         variedDamage, variedTime, pathTotalLength);

            std::cout << "Cromosoma " << i << " evaluado con datos interpolados, fitness: "
                      << population[i].getFitness() << "\n";
        }

        std::cout << "Fitness calculado: " << population[i].getFitness() << "\n";
    }

    // actualizar metrica de diversidad poblacional
    averageDiversity = getDiversityMetric();

    std::cout << "Diversidad promedio: " << averageDiversity << "\n";
    std::cout << "=== FIN DEBUG evaluatePopulation ===\n";
}



// selecciona cromosomas padres para reproduccion usando elitismo y torneo
DynamicArray<Chromosome> Genetics::selectParents() {
    DynamicArray<Chromosome> selectedParents;

    // ordenar poblacion por fitness de mayor a menor
    DynamicArray<Chromosome> sortedPopulation = population;
    std::sort(sortedPopulation.begin(), sortedPopulation.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.getFitness() > b.getFitness();
        });

    // aplicar elitismo conservando los mejores individuos
    int eliteCount = std::max(1, static_cast<int>(populationSize * 0.05f));

    std::cout << "Selección: manteniendo " << eliteCount << " individuos elite\n";

    for (int i = 0; i < eliteCount; i++) {
        selectedParents.push_back(sortedPopulation[i]);
    }

    // calcular suma total de fitness para seleccion por ruleta
    float totalFitness = 0.0f;
    for (const auto& chromosome : population) {
        totalFitness += std::max(0.1f, chromosome.getFitness()); // evitar fitness cero
    }

    // seleccionar el resto usando torneo o ruleta segun diversidad
    const int tournamentSize = 3;
    std::uniform_int_distribution<int> popDist(0, population.size() - 1);

    for (int i = eliteCount; i < populationSize; ++i) {
        if (totalFitness > 0.0f && averageDiversity > convergenceThreshold) {
            // seleccion por ruleta cuando hay suficiente diversidad
            std::uniform_real_distribution<float> dist(0.0f, totalFitness);
            float spinValue = dist(randomGenerator);
            float currentSum = 0.0f;

            for (const auto& chromosome : population) {
                currentSum += std::max(0.1f, chromosome.getFitness());
                if (currentSum >= spinValue) {
                    selectedParents.push_back(chromosome);
                    break;
                }
            }
        } else {
            // seleccion por torneo cuando hay poca diversidad
            Chromosome best = population[popDist(randomGenerator)];

            for (int j = 1; j < tournamentSize; j++) {
                Chromosome candidate = population[popDist(randomGenerator)];
                if (candidate.getFitness() > best.getFitness()) {
                    best = candidate;
                }
            }

            selectedParents.push_back(best);
        }
    }

    // completar seleccion si es necesario
    while (selectedParents.size() < populationSize) {
        selectedParents.push_back(population[popDist(randomGenerator)]);
    }

    return selectedParents;
}



// genera la siguiente generacion mediante seleccion, cruzamiento y mutacion
void Genetics::createNextGeneration() {
    generation++;
    mutationCount = 0;

    std::cout << "=== Creando Generación " << generation << " ===\n";

    DynamicArray<Chromosome> parents = selectParents();
    DynamicArray<Chromosome> newPopulation;

    // conservar mejor cromosoma usando elitismo estricto
    Chromosome bestChromosome = getBestChromosome();
    newPopulation.push_back(bestChromosome);
    std::cout << "Mejor cromosoma conservado con fitness: " << bestChromosome.getFitness() << "\n";

    // ajustar parametros segun estado actual de la poblacion
    adaptParameters();

    // generar resto de la poblacion mediante reproduccion
    std::uniform_real_distribution<float> crossoverChance(0.0f, 1.0f);

    while (newPopulation.size() < populationSize) {
        // seleccionar dos padres diferentes para maximizar diversidad
        std::uniform_int_distribution<int> parentDist(0, parents.size() - 1);
        int parent1Index = parentDist(randomGenerator);
        int parent2Index = parentDist(randomGenerator);

        // intentar obtener padres diferentes
        int attempts = 0;
        while (parent1Index == parent2Index && attempts < 10) {
            parent2Index = parentDist(randomGenerator);
            attempts++;
        }

        Chromosome& parent1 = parents[parent1Index];
        Chromosome& parent2 = parents[parent2Index];

        Chromosome child;

        // aplicar cruzamiento segun probabilidad configurada
        if (crossoverChance(randomGenerator) < crossoverRate) {
            child = parent1.crossover(parent2);
        } else {
            // sin cruzamiento, heredar del padre con mejor fitness
            child = (parent1.getFitness() > parent2.getFitness()) ? parent1 : parent2;
        }

        // detectar y contar mutaciones comparando antes y despues
        Chromosome originalChild = child;
        child.mutate(mutationRate);

        // verificar si ocurrio mutacion en alguna caracteristica
        if (std::abs(child.getHealth() - originalChild.getHealth()) > 0.01f ||
            std::abs(child.getSpeed() - originalChild.getSpeed()) > 0.01f ||
            std::abs(child.getArrowResistance() - originalChild.getArrowResistance()) > 0.01f ||
            std::abs(child.getArtilleryResistance() - originalChild.getArtilleryResistance()) > 0.01f ||
            std::abs(child.getMagicResistance() - originalChild.getMagicResistance()) > 0.01f) {
            mutationCount++;
        }

        newPopulation.push_back(child);
    }

    // reemplazar poblacion anterior con la nueva generacion
    population = newPopulation;

    // aplicar mecanismos de mantenimiento de diversidad si es necesario
    if (averageDiversity < convergenceThreshold) {
        maintainDiversity();
    }

    std::cout << "Nueva generación creada. Mutaciones: " << mutationCount
              << "/" << populationSize << " (" << (100.0f * mutationCount / populationSize) << "%)\n";
    std::cout << "=== Fin Generación " << generation << " ===\n";
}



// encuentra y retorna el cromosoma con mayor fitness
Chromosome Genetics::getBestChromosome() const {
    if (population.empty()) {
        return Chromosome();
    }

    return *std::max_element(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.getFitness() < b.getFitness();
    });
}



// selecciona cromosomas para generar enemigos en la siguiente oleada
DynamicArray<Chromosome> Genetics::getChromosomesForWave(int count) {
    DynamicArray<Chromosome> waveChromosomes;

    // generar cromosomas aleatorios si no hay poblacion disponible
    if (population.empty()) {
        for (int i = 0; i < count; ++i) {
            waveChromosomes.push_back(Chromosome());
        }
        return waveChromosomes;
    }

    // ordenar poblacion por fitness para seleccion estratificada
    DynamicArray<Chromosome> sortedPopulation = population;
    std::sort(sortedPopulation.begin(), sortedPopulation.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.getFitness() > b.getFitness();
    });

    // asegurar que usamos todos los cromosomas solicitados
    for (int i = 0; i < count; ++i) {
        if (i < sortedPopulation.size()) {
            // usar cromosomas existentes en orden de fitness
            waveChromosomes.push_back(sortedPopulation[i]);
        } else {
            // si necesitamos más cromosomas de los que tenemos, usar los mejores con mutación
            int sourceIndex = i % sortedPopulation.size();
            Chromosome mutatedChromosome = sortedPopulation[sourceIndex];
            mutatedChromosome.mutate(0.3f); // mayor mutación para crear diversidad
            waveChromosomes.push_back(mutatedChromosome);
        }
    }

    std::cout << "Cromosomas seleccionados para oleada: " << waveChromosomes.size()
              << " (necesarios: " << count << ")\n";

    return waveChromosomes;
}



// recopila todos los valores de fitness de la poblacion actual
DynamicArray<float> Genetics::getCurrentFitnessScores() const {
    DynamicArray<float> scores;

    for (const auto& chromosome : population) {
        scores.push_back(chromosome.getFitness());
    }

    return scores;
}



// calcula el fitness promedio de toda la poblacion
float Genetics::getAverageFitness() const {
    if (population.empty()) {
        return 0.0f;
    }

    float sumFitness = 0.0f;
    for (const auto& chromosome : population) {
        sumFitness += chromosome.getFitness();
    }

    return sumFitness / population.size();
}



// mide la diversidad genetica promedio de la poblacion
float Genetics::getDiversityMetric() const {
    if (population.size() < 2) {
        return 1.0f;
    }

    float totalDiversity = 0.0f;
    int comparisons = 0;

    // calcular diversidad entre pares de cromosomas con limite de comparaciones
    for (size_t i = 0; i < population.size(); ++i) {
        for (size_t j = i + 1; j < population.size() && comparisons < 50; ++j) {
            totalDiversity += population[i].calculateDiversity(population[j]);
            comparisons++;
        }
    }

    return comparisons > 0 ? totalDiversity / comparisons : 1.0f;
}



// determina si la poblacion ha convergido a una solucion similar
bool Genetics::hasConverged() const {
    return averageDiversity < convergenceThreshold;
}



// ajusta dinamicamente los parametros del algoritmo segun el progreso
void Genetics::adaptParameters() {
    // incrementar mutacion si la diversidad es muy baja
    if (averageDiversity < convergenceThreshold) {
        mutationRate = std::min(0.3f, mutationRate * 1.2f);
        std::cout << "Baja diversidad detectada. Tasa de mutación aumentada a: " << mutationRate << "\n";
    } else if (averageDiversity > 0.8f) {
        // reducir mutacion si hay exceso de diversidad
        mutationRate = std::max(0.05f, mutationRate * 0.9f);
        std::cout << "Alta diversidad. Tasa de mutación reducida a: " << mutationRate << "\n";
    }

    // ajustar cruzamiento basado en calidad promedio de la poblacion
    float avgFitness = getAverageFitness();
    if (avgFitness < 100.0f) {
        // poblacion debil necesita mas exploracion
        crossoverRate = std::min(0.9f, crossoverRate * 1.1f);
    } else if (avgFitness > 400.0f) {
        // poblacion fuerte necesita mas conservacion
        crossoverRate = std::max(0.5f, crossoverRate * 0.95f);
    }
}



// introduce nuevos individuos aleatorios para mantener diversidad genetica
void Genetics::maintainDiversity() {
    int newIndividuals = populationSize * 0.2f; // reemplazar 20% de la poblacion
    std::uniform_int_distribution<int> popDist(0, population.size() - 1);

    std::cout << "Manteniendo diversidad: introduciendo " << newIndividuals << " nuevos individuos\n";

    // ordenar poblacion por fitness para reemplazar a los peores
    std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.getFitness() > b.getFitness();
    });

    // sustituir los peores individuos con cromosomas completamente nuevos
    for (int i = 0; i < newIndividuals && i < population.size(); ++i) {
        int replaceIndex = population.size() - 1 - i;
        population[replaceIndex] = Chromosome(); // generar cromosoma aleatorio
    }
}



// reinicia parcialmente la poblacion si no hay mejoras por muchas generaciones
void Genetics::resetPopulationIfStagnant() {
    static int stagnantGenerations = 0;
    static float lastBestFitness = 0.0f;

    float currentBestFitness = getBestChromosome().getFitness();

    // detectar estancamiento comparando con fitness anterior
    if (std::abs(currentBestFitness - lastBestFitness) < 5.0f) {
        stagnantGenerations++;
    } else {
        stagnantGenerations = 0;
    }

    lastBestFitness = currentBestFitness;

    // aplicar reinicio parcial despues de mucho estancamiento
    if (stagnantGenerations > 10) {
        std::cout << "Población estancada por " << stagnantGenerations
                  << " generaciones. Reseteando 50% de la población.\n";

        // conservar solo la mitad superior de la poblacion
        std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.getFitness() > b.getFitness();
        });

        int keepCount = population.size() / 2;
        for (size_t i = keepCount; i < population.size(); ++i) {
            population[i] = Chromosome(); // reemplazar con cromosomas frescos
        }

        stagnantGenerations = 0;
        mutationRate = 0.15f; // restaurar tasa de mutacion por defecto
        crossoverRate = 0.7f; // restaurar tasa de cruzamiento por defecto
    }
}



// obtiene el numero de la generacion actual
int Genetics::getGeneration() const {
    return generation;
}



// obtiene cuantas mutaciones ocurrieron en la ultima generacion
int Genetics::getMutationCount() const {
    return mutationCount;
}



// obtiene la tasa de mutacion configurada actualmente
float Genetics::getMutationRate() const {
    return mutationRate;
}



// establece la longitud total del camino para calculos de fitness
void Genetics::setPathTotalLength(float length) {
    pathTotalLength = length;
    std::cout << "Longitud total del camino establecida: " << length << "\n";
}