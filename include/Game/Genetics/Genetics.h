#pragma once

#include "Chromosome.h"
#include <vector>
#include <random>
#include <memory>

class Genetics {
public:
    Genetics(int populationSize, float mutationRate, float crossoverRate);
    void initializePopulation();
    void evaluatePopulation(const std::vector<bool>& reachedEnd, const std::vector<float>& distancesTraveled, const std::vector<float>& damagesDealt, const std::vector<float>& timesAlive);
    std::vector<Chromosome> selectParents();
    void createNextGeneration();
    Chromosome getBestChromosome() const;
    std::vector<Chromosome> getChromosomesForWave(int count);
    int getGeneration() const;
    float getAverageFitness() const;
    int getMutationCount() const;
    float getMutationRate() const;

private:
    std::vector<Chromosome> population;
    int populationSize;
    float mutationRate;
    float crossoverRate;
    int generation;
    int mutationCount;
    std::mt19937 randomGenerator;
};