#include "../include/Game/Genetics//Genetics.h"
#include <algorithm>
#include <numeric>
#include <iostream>

// constructor
Genetics::Genetics(int populationSize, float mutationRate, float crossoverRate)
    : populationSize(populationSize), mutationRate(mutationRate), crossoverRate(crossoverRate),
      generation(0), mutationCount(0) {

    // inicializar el generador con una semilla aleatoria
    std::random_device rd;
    randomGenerator = std::mt19937(rd());

    // inicializar la población
    initializePopulation();
}



// inicializar la poblacion con cromosomas aleatorios
void Genetics::initializePopulation() {
    population.clear();

    for (int i = 0; i < populationSize; ++i) {
        population.push_back(Chromosome());
    }
}



// evaluar la poblacion actual
void Genetics::evaluatePopulation(const DynamicArray<bool> &reachedEnd, const DynamicArray<float> &distancesTraveled, const DynamicArray<float> &damagesDealt, const DynamicArray<float> &timesAlive) {
    if (reachedEnd.size() != distancesTraveled.size() || reachedEnd.size() != damagesDealt.size() || reachedEnd.size() != timesAlive.size() || reachedEnd.size() != population.size()) {
        return;
    }

    // evaluar cada cromosoma
    for (size_t i = 0; i < population.size(); ++i) {
        population[i].calculateFitness(reachedEnd[i], distancesTraveled[i], damagesDealt[i], timesAlive[i]);
    }
}



// seleccionar padres usando seleccion por ruleta
DynamicArray<Chromosome> Genetics::selectParents() {
    DynamicArray<Chromosome> selectedParents;

    // calcular la suma total de fitness
    float totalFitness = 0.0f;
    for (const auto& chromosome : population) {
        totalFitness += chromosome.getFitness();
    }

    // si el fitness total es cero, selecciona padres aleatorios
    if (totalFitness <= 0.0f) {
        std::uniform_int_distribution<int> dist(0, population.size() - 1);
        for (int i = 0; i < populationSize; ++i) {
            selectedParents.push_back(population[dist(randomGenerator)]);
        }
        return selectedParents;
    }

    // seleccion por ruleta
    std::uniform_real_distribution<float> dist(0.0f, totalFitness);
    for (int i = 0; i < populationSize; ++i) {
        float spinValue = dist(randomGenerator);
        float currentSum = 0.0f;

        for (const auto& chromosome : population) {
            currentSum += chromosome.getFitness();
            if (currentSum >= spinValue) {
                selectedParents.push_back(chromosome);
                break;
            }
        }
    }
    return selectedParents;
}



// crear la siguiente generacion
void Genetics::createNextGeneration() {
    generation++; // incrementar la generacion
    mutationCount = 0; // resetea el contador de mutaciones
    DynamicArray<Chromosome> parents = selectParents(); // seleccionar padres
    DynamicArray<Chromosome> newPopulation; // crear nueva poblacion

    // conservar el mejor cromosoma
    Chromosome bestChromosome = getBestChromosome();
    newPopulation.push_back(bestChromosome);

    // crear el resto de la población
    std::uniform_real_distribution<float> crossoverChance(0.0f, 1.0f);

    while (newPopulation.size() < populationSize) {
        // seleccionar dos padres aleatorios
        std::uniform_int_distribution<int> parentDist(0, parents.size() - 1);
        int parent1Random = parentDist(randomGenerator);
        int parent2Random = parentDist(randomGenerator);
        Chromosome& parent1 = parents[parent1Random];
        Chromosome& parent2 = parents[parent2Random];

        Chromosome child;

        // aplicar crossover si se cumple la probabilidad
        if (crossoverChance(randomGenerator) < crossoverRate) {
            child = parent1.crossover(parent2);
        } else {
            // sin crossover, simplemente copia uno de los padres
            child = (crossoverChance(randomGenerator) < 0.5f) ? parent1 : parent2;
        }

        bool mutated = false;
        Chromosome originalChild = child;
        child.mutate(mutationRate);

        // comprobar si hubo mutacion
        if (child.getHealth() != originalChild.getHealth() || child.getSpeed() != originalChild.getSpeed() || child.getArrowResistance() != originalChild.getArrowResistance() || child.getArtilleryResistance() != originalChild.getArtilleryResistance() || child.getMagicResistance() != originalChild.getMagicResistance()) {
            mutationCount++;
        }

        newPopulation.push_back(child);
    }

    // reemplazar la poblacion antigua
    population = newPopulation;
}



// obtener el mejor cromosoma de la generacion actual
Chromosome Genetics::getBestChromosome() const {
    if (population.empty()) {
        return Chromosome();
    }

    return *std::max_element(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.getFitness() < b.getFitness();
    });
}



// obtener cromosomas para la siguiente oleada
DynamicArray<Chromosome> Genetics::getChromosomesForWave(int count) {
    DynamicArray<Chromosome> waveChromosomes;

    // si no hay suficientes cromosomas en la poblacion, devolver cromosomas aleatorios
    if (population.empty()) {
        for (int i = 0; i < count; ++i) {
            waveChromosomes.push_back(Chromosome());
        }
        return waveChromosomes;
    }

    // ordenar la poblacion por fitness
    DynamicArray<Chromosome> sortedPopulation = population;
    std::sort(sortedPopulation.begin(), sortedPopulation.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.getFitness() > b.getFitness();
    });

    // seleccionar los mejores cromosomas
    for (int i = 0; i < count && i < sortedPopulation.size(); ++i) {
        waveChromosomes.push_back(sortedPopulation[i]);
    }

    // si se necesitan mas cromsomas de los que hay en la poblacion
    while (waveChromosomes.size() < count) {
        waveChromosomes.push_back(Chromosome());
    }

    return waveChromosomes;
}



// obtener el fitness promedio
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



// obtener la generacion actual
int Genetics::getGeneration() const {
    return generation;
}

// obtener el numero de mutaciones en la ultima generacion
int Genetics::getMutationCount() const {
    return mutationCount;
}

// obtener la tasa de mutacion actual
float Genetics::getMutationRate() const {
    return mutationRate;
}
