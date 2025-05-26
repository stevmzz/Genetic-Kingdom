#include "../include/Game/Genetics/Chromosome.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <cmath>

// generador de numeros aleatorios compartido por todos los cromosomas
std::mt19937 Chromosome::randomGenerator(static_cast<unsigned int>(std::time(nullptr)));



// crea un cromosoma con valores aleatorios dentro de rangos validos
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



// crea un cromosoma con valores especificos proporcionados
Chromosome::Chromosome(float health, float speed, float arrowRes, float magicRes, float artilleryRes)
    : health(health), speed(speed), arrowResistance(arrowRes),
      magicResistance(magicRes), artilleryResistance(artilleryRes), fitness(0.0f) {
    clampValues();
}



// crea una copia exacta de otro cromosoma
Chromosome::Chromosome(const Chromosome& other)
    : health(other.health), speed(other.speed),
      arrowResistance(other.arrowResistance),
      magicResistance(other.magicResistance),
      artilleryResistance(other.artilleryResistance),
      fitness(other.fitness) {
}



// evalua que tan bueno es este cromosoma basado en rendimiento del enemigo
void Chromosome::calculateFitness(bool reachedEnd, float distanceTraveled, float damageDealt, float timeAlive, float pathTotalLength) {
    // normalizar metricas para evitar que una domine sobre las otras
    float normalizedDistance = pathTotalLength > 0 ? std::min(distanceTraveled / pathTotalLength, 1.0f) : 0.0f;
    float normalizedTime = std::min(timeAlive / 30.0f, 1.0f); // tiempo maximo esperado 30 segundos
    float normalizedDamage = std::min(damageDealt / 100.0f, 1.0f); // daño esperado ajustable

    fitness = 0.0f;

    // recompensa principal por completar el objetivo
    if (reachedEnd) {
        fitness += 500.0f;
    }

    // puntos por distancia recorrida (progreso hacia el objetivo)
    fitness += normalizedDistance * 200.0f; // maximo 200 puntos

    // puntos por tiempo de supervivencia
    fitness += normalizedTime * 100.0f; // maximo 100 puntos

    // puntos por daño infligido si esta disponible
    fitness += normalizedDamage * 50.0f;

    // bono por combinacion efectiva de supervivencia y progreso
    if (normalizedDistance > 0.7f && normalizedTime > 0.5f) {
        fitness += 100.0f;
    }

    // bono adicional por supervivencia prolongada sin completar
    if (!reachedEnd && timeAlive > 15.0f) {
        fitness += 30.0f * (timeAlive / 30.0f);
    }

    // penalizacion por valores fuera de rangos validos
    if (!isValid()) {
        fitness *= 0.5f;
    }

    // mostrar informacion detallada del calculo de fitness
    std::cout << "Fitness calculado: " << fitness
              << " (distancia: " << normalizedDistance * 100 << "%"
              << ", tiempo: " << timeAlive << "s"
              << ", llegó: " << (reachedEnd ? "SÍ" : "NO") << ")\n";
}



// combina genes de dos cromosomas para crear descendencia
Chromosome Chromosome::crossover(const Chromosome &other) const {
    std::uniform_real_distribution<float> weightDist(0.2f, 0.8f);
    std::uniform_int_distribution<int> methodDist(0, 1);

    Chromosome child;

    // elegir metodo de cruzamiento aleatoriamente
    if (methodDist(randomGenerator) == 0) {
        // cruzamiento aritmetico - mezcla ponderada de genes
        float w1 = weightDist(randomGenerator);
        float w2 = 1.0f - w1;

        child = Chromosome(
            health * w1 + other.health * w2,
            speed * w1 + other.speed * w2,
            arrowResistance * w1 + other.arrowResistance * w2,
            magicResistance * w1 + other.magicResistance * w2,
            artilleryResistance * w1 + other.artilleryResistance * w2
        );
    } else {
        // cruzamiento de un punto - corte en posicion aleatoria
        std::uniform_int_distribution<int> pointDist(1, 4);
        int crossPoint = pointDist(randomGenerator);

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
    }

    child.clampValues();
    return child;
}



// aplica mutaciones aleatorias para mantener diversidad genetica
void Chromosome::mutate(float mutationRate) {
    std::uniform_real_distribution<float> mutationChance(0.0f, 1.0f);

    // ajustar intensidad de mutacion segun fitness actual
    float adaptiveFactor = (fitness < 100.0f) ? 2.0f :
                          (fitness < 300.0f) ? 1.2f : 0.8f;

    // distribuciones normales para cambios graduales
    std::normal_distribution<float> healthMut(0.0f, 40.0f * adaptiveFactor);
    std::normal_distribution<float> speedMut(0.0f, 15.0f * adaptiveFactor);
    std::normal_distribution<float> resMut(0.0f, 0.25f * adaptiveFactor);

    // aplicar mutacion a cada gen segun probabilidad
    if (mutationChance(randomGenerator) < mutationRate) {
        health += healthMut(randomGenerator);
        health = std::max(50.0f, std::min(350.0f, health));
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        speed += speedMut(randomGenerator);
        speed = std::max(20.0f, std::min(120.0f, speed));
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        arrowResistance += resMut(randomGenerator);
        arrowResistance = std::clamp(arrowResistance, 0.0f, 2.5f);
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        magicResistance += resMut(randomGenerator);
        magicResistance = std::clamp(magicResistance, 0.0f, 2.5f);
    }

    if (mutationChance(randomGenerator) < mutationRate) {
        artilleryResistance += resMut(randomGenerator);
        artilleryResistance = std::clamp(artilleryResistance, 0.0f, 2.5f);
    }

    clampValues();
}



// mide que tan diferentes son dos cromosomas para diversidad
float Chromosome::calculateDiversity(const Chromosome& other) const {
    float totalDiff = 0.0f;

    // calcular diferencias normalizadas para cada caracteristica
    totalDiff += std::abs(health - other.health) / 300.0f;
    totalDiff += std::abs(speed - other.speed) / 100.0f;
    totalDiff += std::abs(arrowResistance - other.arrowResistance) / 2.0f;
    totalDiff += std::abs(magicResistance - other.magicResistance) / 2.0f;
    totalDiff += std::abs(artilleryResistance - other.artilleryResistance) / 2.0f;

    return totalDiff / 5.0f; // promedio de las diferencias
}



// verifica que todos los valores esten dentro de rangos aceptables
bool Chromosome::isValid() const {
    return health >= 50.0f && health <= 350.0f &&
           speed >= 20.0f && speed <= 120.0f &&
           arrowResistance >= 0.0f && arrowResistance <= 2.5f &&
           magicResistance >= 0.0f && magicResistance <= 2.5f &&
           artilleryResistance >= 0.0f && artilleryResistance <= 2.5f;
}



// fuerza todos los valores a estar dentro de rangos validos
void Chromosome::clampValues() {
    health = std::clamp(health, 50.0f, 350.0f);
    speed = std::clamp(speed, 20.0f, 120.0f);
    arrowResistance = std::clamp(arrowResistance, 0.0f, 2.5f);
    magicResistance = std::clamp(magicResistance, 0.0f, 2.5f);
    artilleryResistance = std::clamp(artilleryResistance, 0.0f, 2.5f);
}



// getters
float Chromosome::getHealth() const { return health; }
float Chromosome::getSpeed() const { return speed; }
float Chromosome::getArrowResistance() const { return arrowResistance; }
float Chromosome::getMagicResistance() const { return magicResistance; }
float Chromosome::getArtilleryResistance() const { return artilleryResistance; }
float Chromosome::getFitness() const { return fitness; }