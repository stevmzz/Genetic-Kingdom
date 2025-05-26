#pragma once

#include "Chromosome.h"
#include "../include/DataStructures/DynamicArray.h"
#include <random>

class Genetics {
public:
    Genetics(int populationSize, float mutationRate, float crossoverRate);

    void initializePopulation();
    void evaluatePopulation(const DynamicArray<bool>& reachedEnd, const DynamicArray<float>& distancesTraveled, const DynamicArray<float>& damagesDealt, const DynamicArray<float>& timesAlive);
    DynamicArray<Chromosome> selectParents();
    void createNextGeneration();
    Chromosome getBestChromosome() const;
    DynamicArray<Chromosome> getChromosomesForWave(int count);
    DynamicArray<float> getCurrentFitnessScores() const;
    float getAverageFitness() const;
    float getDiversityMetric() const;
    bool hasConverged() const;
    int getGeneration() const;
    int getMutationCount() const;
    float getMutationRate() const;
    void setPathTotalLength(float length);
    void adaptParameters();
    void maintainDiversity();
    void resetPopulationIfStagnant();

private:
    DynamicArray<Chromosome> population;
    int populationSize;
    float mutationRate;
    float crossoverRate;
    int generation;
    int mutationCount;
    float pathTotalLength;
    std::mt19937 randomGenerator;
    float averageDiversity;
    float convergenceThreshold;
};