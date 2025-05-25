#pragma once

#include "Chromosome.h"
#include "../include/DataStructures/DynamicArray.h"
#include <vector>
#include <random>
#include <memory>

class Genetics {
public:
    Genetics(int populationSize, float mutationRate, float crossoverRate);
    void initializePopulation();
    void evaluatePopulation(const DynamicArray<bool>& reachedEnd, const DynamicArray<float>& distancesTraveled, const DynamicArray<float>& damagesDealt, const DynamicArray<float>& timesAlive);
    DynamicArray<Chromosome> selectParents();
    void createNextGeneration();
    Chromosome getBestChromosome() const;
    DynamicArray<Chromosome> getChromosomesForWave(int count);
    int getGeneration() const;
    float getAverageFitness() const;
    int getMutationCount() const;
    float getMutationRate() const;

private:
    DynamicArray<Chromosome> population;
    int populationSize;
    float mutationRate;
    float crossoverRate;
    int generation;
    int mutationCount;
    std::mt19937 randomGenerator;
};