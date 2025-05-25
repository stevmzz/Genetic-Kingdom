#include "StatsPanel.h"
#include <sstream>
#include <iomanip>

StatsPanel::StatsPanel(sf::Font& font) : isVisible(true) {
    background.setSize(sf::Vector2f(800, 150));
    background.setFillColor(sf::Color(0, 0, 0, 150));
    background.setPosition(10, 10);

    statsText.setFont(font);
    statsText.setCharacterSize(16);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(20, 20);
}

void StatsPanel::setVisible(bool visible) {
    isVisible = visible;
}

void StatsPanel::update(int generation, int enemiesKilled, const std::vector<float>& fitnessList, 
                        const std::vector<int>& towerLevels, float mutationProb, int mutationCount) {
    statsText.setString(formatStats(generation, enemiesKilled, fitnessList, towerLevels, mutationProb, mutationCount));
}

void StatsPanel::draw(sf::RenderWindow& window) {
    if (isVisible) {
        window.draw(background);
        window.draw(statsText);
    }
}

std::string StatsPanel::formatStats(int generation, int enemiesKilled, const std::vector<float>& fitnessList,
                                    const std::vector<int>& towerLevels, float mutationProb, int mutationCount) {
    std::ostringstream oss;
    oss << "Generaciones: " << generation << "\n";
    oss << "Enemigos muertos: " << enemiesKilled << "\n";

    oss << "Fitness: ";
    for (float f : fitnessList) {
        oss << std::fixed << std::setprecision(2) << f << " ";
    }
    oss << "\n";

    oss << "Nivel torres: ";
    for (int lvl : towerLevels) {
        oss << lvl << " ";
    }
    oss << "\n";

    oss << "Mutaciones: " << mutationCount << " (Prob: " << std::fixed << std::setprecision(2) << (mutationProb * 100) << "%)";
    return oss.str();
}