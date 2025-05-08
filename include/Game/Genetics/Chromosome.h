#pragma once

#include <vector>
#include <random>

class Chromosome {
public:
  Chromosome();
  Chromosome(float health, float speed, float arrowRes, float magicRes, float artilleryRes);
  Chromosome(const Chromosome& other);
  void calculateFitness(bool reachedEnd, float distanceTraveled, float damageDealt, float timeAlive);
  Chromosome crossover(const Chromosome& other) const;
  void mutate(float mutationRate);
  float getHealth() const;
  float getSpeed() const;
  float getArrowResistance() const;
  float getMagicResistance() const;
  float getArtilleryResistance() const;
  float getFitness() const;

private:
  float health;
  float speed;
  float arrowResistance;
  float magicResistance;
  float artilleryResistance;
  float fitness;
  static std::mt19937 randomGenerator;
};