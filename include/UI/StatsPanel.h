#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class StatsPanel {
public:
    StatsPanel(sf::Font& font);
    void update(int generation, int enemiesKilled, const std::vector<float>& fitnessList, 
                const std::vector<int>& towerLevels, float mutationProb, int mutationCount);
    void draw(sf::RenderWindow& window);
    void setVisible(bool visible);

private:
    bool isVisible;
    sf::RectangleShape background;
    sf::Text statsText;

    std::string formatStats(int generation, int enemiesKilled, const std::vector<float>& fitnessList,
                            const std::vector<int>& towerLevels, float mutationProb, int mutationCount);
};