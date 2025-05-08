#include "../include/Game/Genetics/Chromosome.h"
#include <algorithm>
#include <ctime>

// inicializar el generador de números aleatorios
std::mt19937 Chromosome::randomGenerator(static_cast<unsigned int>(std::time(nullptr)));



// constructor por defecto (crea genes aleatorios)
Chromosome::Chromosome() {
    std::uniform_real_distribution<float> healthDist(100.0f, 300.0f);
    std::uniform_real_distribution<float> speedDist(40.0f, 100.0f);
    std::uniform_real_distribution<float> resistanceDist(0.0f, 2.0f);

    health = healthDist(randomGenerator);
    speed = speedDist(randomGenerator);
    arrowResistance = resistanceDist(randomGenerator);
    magicResistance = resistanceDist(randomGenerator);
    artilleryResistance = resistanceDist(randomGenerator);

    fitness = 0.0f;
}



// constructor con valores específicos
Chromosome::Chromosome(float health, float speed, float arrowRes, float magicRes, float artilleryRes)
    : health(health), speed(speed), arrowResistance(arrowRes),
      magicResistance(magicRes), artilleryResistance(artilleryRes), fitness(0.0f) {
}



// constructor de copia
Chromosome::Chromosome(const Chromosome& other)
    : health(other.health), speed(other.speed),
      arrowResistance(other.arrowResistance),
      magicResistance(other.magicResistance),
      artilleryResistance(other.artilleryResistance),
      fitness(other.fitness) {
}



// calcula el fitness del cromosoma
void Chromosome::calculateFitness(bool reachedEnd, float distanceTraveled, float damageDealt, float timeAlive) {
    // pesos para diferentes factores
    const float endWeight = 100.0f;
    const float distanceWeight = 1.0f;
    const float damageWeight = 0.5f;
    const float timeWeight = 0.1f;

    // calcular fitness
    fitness = 0.0f;

    // bonus por llegar al final
    if (reachedEnd) {
        fitness += endWeight;
    }

    // aumenta el fitness segun la distancia recorrida
    fitness += distanceWeight * distanceTraveled;

    // aumenta el fitness segun el daño que cuasó al jugador
    fitness += damageWeight * damageDealt;

    // aumenta el fitness segun  el tiempo vivo
    fitness += timeWeight * timeAlive;
}



// operador de cruce (crossover)
Chromosome Chromosome::crossover(const Chromosome &other) const {
    // crossover de un punto aleatorio
    std::uniform_int_distribution<int> pointDist(1, 4); // 5 genes, así que 4 posibles puntos de cruce
    int crossPoint = pointDist(randomGenerator);

    // nuevo cromosoma para el hijo
    Chromosome child;

    // realizar el cruce basado en el punto aleatorio
    switch (crossPoint) {
        case 1:
            child = Chromosome(other.health, speed, arrowResistance, magicResistance, artilleryResistance);
            break;
        case 2:
            child = Chromosome(other.health, other.speed, arrowResistance, magicResistance, artilleryResistance);
            break;
        case 3:
            child = Chromosome(other.health, other.speed, other.arrowResistance, magicResistance, artilleryResistance);
            break;
        case 4:
            child = Chromosome(other.health, other.speed, other.arrowResistance, other.magicResistance, artilleryResistance);
            break;
    }
    return child;
}



// operador de mutacion
void Chromosome::mutate(float mutationRate) {
    std::uniform_real_distribution<float> mutationChance(0.0f, 1.0f);

    // distribuciones para cambios en caso de mutación
    std::uniform_real_distribution<float> healthMut(-50.0f, 50.0f);
    std::uniform_real_distribution<float> speedMut(-10.0f, 10.0f);
    std::uniform_real_distribution<float> resMut(-0.2f, 0.2f);

    // por cada gen, decide si muta
    if (mutationChance(randomGenerator) < mutationRate) {
        health = std::max(10.0f, health + healthMut(randomGenerator));
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        speed = std::max(10.0f, speed + speedMut(randomGenerator));
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        arrowResistance = std::clamp(arrowResistance + resMut(randomGenerator), 0.1f, 2.0f);
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        magicResistance = std::clamp(magicResistance + resMut(randomGenerator), 0.1f, 2.0f);
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        artilleryResistance = std::clamp(artilleryResistance + resMut(randomGenerator), 0.1f, 2.0f);
    }
}



// getters
float Chromosome::getHealth() const { return health; }
float Chromosome::getSpeed() const { return speed; }
float Chromosome::getArrowResistance() const { return arrowResistance; }
float Chromosome::getMagicResistance() const { return magicResistance; }
float Chromosome::getArtilleryResistance() const { return artilleryResistance; }
float Chromosome::getFitness() const { return fitness; }


