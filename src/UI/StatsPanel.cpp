#include "../include/UI/StatsPanel.h"
#include <sstream>
#include <iomanip>

// inicializa el panel de estadisticas con fuente y configuracion visual
StatsPanel::StatsPanel(const sf::Font& font) : isVisible(true) {
    // configurar fondo del panel con tamaño amplio para dos columnas
    background.setSize(sf::Vector2f(1600, 185));
    background.setFillColor(sf::Color(0, 0, 0, 150));

    // posicionar en la parte inferior central de la pantalla
    background.setPosition(160, 880);

    // configurar texto de estadisticas
    statsText.setFont(font);
    statsText.setCharacterSize(16);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(180, 890);
}



// controla si el panel debe mostrarse o permanecer oculto
void StatsPanel::setVisible(bool visible) {
    isVisible = visible;
}



// actualiza el contenido del panel con nuevas estadisticas
void StatsPanel::update(int generation, int enemiesKilledThisWave, const DynamicArray<float>& currentWaveFitnessList, float mutationProb, int mutationCount) {
    statsText.setString(formatStats(generation, enemiesKilledThisWave, currentWaveFitnessList, mutationProb, mutationCount));
}



// dibuja el panel en pantalla si esta visible
void StatsPanel::draw(sf::RenderWindow& window) {
    if (isVisible) {
        window.draw(background);
        window.draw(statsText);
    }
}



// prepara el panel para una nueva oleada de enemigos
void StatsPanel::resetForNewWave() {
    // el metodo formatstats maneja automaticamente el reinicio de datos
    // no se requiere logica adicional aqui
}



// formatea las estadisticas en un layout de dos columnas legible
std::string StatsPanel::formatStats(int generation, int enemiesKilledThisWave, const DynamicArray<float>& currentWaveFitnessList, float mutationProb, int mutationCount) {
    std::ostringstream oss;

    // preparar contenido de la columna izquierda con informacion general
    DynamicArray<std::string> leftColumn;
    leftColumn.push_back("Generation: " + std::to_string(generation));
    leftColumn.push_back("Death enemies: " + std::to_string(enemiesKilledThisWave));

    // calcular el porcentaje real de mutaciones
    float realMutationPercentage = 0.0f;
    if (generation > 0) {
        realMutationPercentage = (static_cast<float>(mutationCount) / 20.0f) * 100.0f; // 20 = tamaño población
    }

    // formatear linea de mutaciones con porcentaje REAL
    std::ostringstream mutationLine;
    mutationLine << "Mutations: " << mutationCount << "/" << (generation > 0 ? 20 : 0)
                 << " (" << static_cast<int>(realMutationPercentage) << "%)";
    leftColumn.push_back(mutationLine.str());

    // preparar contenido de la columna derecha con valores de fitness
    DynamicArray<std::string> rightColumn;
    rightColumn.push_back("Fitness:");

    if (currentWaveFitnessList.empty()) {
        rightColumn.push_back("(No deaths yet)");
    } else {
        // listar cada valor de fitness con numeracion
        for (size_t i = 0; i < currentWaveFitnessList.size(); ++i) {
            std::ostringstream fitnessLine;
            fitnessLine << (i+1) << ". " << std::fixed << std::setprecision(1) << currentWaveFitnessList[i];
            rightColumn.push_back(fitnessLine.str());
        }
    }

    // combinar ambas columnas linea por linea para crear layout final
    size_t maxLines = (leftColumn.size() > rightColumn.size()) ? leftColumn.size() : rightColumn.size();

    for (size_t i = 0; i < maxLines; ++i) {
        // escribir columna izquierda con ancho fijo de 40 caracteres
        if (i < leftColumn.size()) {
            oss << std::left << std::setw(40) << leftColumn[i];
        } else {
            oss << std::setw(40) << ""; // rellenar con espacios vacios
        }

        // escribir columna derecha sin restriccion de ancho
        if (i < rightColumn.size()) {
            oss << rightColumn[i];
        }

        // agregar salto de linea excepto en la ultima fila
        if (i < maxLines - 1) {
            oss << "\n";
        }
    }

    return oss.str();
}