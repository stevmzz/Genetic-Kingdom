#pragma once
#include <SFML/Graphics.hpp>
#include "../DataStructures/DynamicArray.h"
#include <string>

class StatsPanel {
public:
    StatsPanel(const sf::Font& font);
    void update(int generation, int enemiesKilled, const DynamicArray<float>& fitnessList, float mutationProb, int mutationCount);
    void draw(sf::RenderWindow& window);
    void setVisible(bool visible);
    void resetForNewWave();

private:
    bool isVisible;
    sf::RectangleShape background;
    sf::Text statsText;

    std::string formatStats(int generation, int enemiesKilled, const DynamicArray<float>& fitnessList, float mutationProb, int mutationCount);
};