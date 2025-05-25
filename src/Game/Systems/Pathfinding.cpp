#include "../include/Game/Systems/Pathfinding.h"
#include "../include/Game/Grid/Grid.h"
#include "../include/Game/Grid/Cell.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// calcula la heuristica usando distancia manhattan
float Pathfinding::calculateHeuristic(int r1, int c1, int r2, int c2) {
   return static_cast<float>(std::abs(r1 - r2) + std::abs(c1 - c2));
}



// verifica si una celda esta dentro de los limites del grid
bool Pathfinding::isValidCell(int row, int col, Grid* grid) {
   return row >= 0 && row < grid->getRows() && col >= 0 && col < grid->getCols();
}



// verifica si una celda es transitable (no tiene torre)
bool Pathfinding::isCellWalkable(int row, int col, Grid* grid) {
   if (!isValidCell(row, col, grid)) {
       return false;
   }

   Cell* cell = grid->getCellAt(row, col);
   if (!cell) {
       return false;
   }

   // una celda es transitable si no tiene una torre
   return !cell->hasTower();
}



// obtiene los vecinos validos de una celda (4 direcciones)
DynamicArray<std::pair<int, int>> Pathfinding::getNeighbors(int row, int col, Grid* grid) {
   DynamicArray<std::pair<int, int>> neighbors;

   // 4 direcciones: arriba, abajo, izquierda, derecha
   const int dr[] = {-1, 0, 1, 0};
   const int dc[] = {0, 1, 0, -1};

   for (int i = 0; i < 4; ++i) {
       int newRow = row + dr[i];
       int newCol = col + dc[i];

       if (isCellWalkable(newRow, newCol, grid)) {
           neighbors.push_back({newRow, newCol});
       }
   }

   return neighbors;
}



// convierte coordenadas del mundo a indices del grid
std::pair<int, int> Pathfinding::worldToGrid(const sf::Vector2f& worldPos, Grid* grid) {
   float gridX = grid->getX();
   float gridY = grid->getY();
   float cellSize = grid->getCellSize();

   int col = static_cast<int>((worldPos.x - gridX) / cellSize);
   int row = static_cast<int>((worldPos.y - gridY) / cellSize);

   // clamp a los limites del grid
   col = std::max(0, std::min(col, grid->getCols() - 1));
   row = std::max(0, std::min(row, grid->getRows() - 1));

   return {row, col};
}



// convierte indices del grid a coordenadas del mundo (centro de la celda)
sf::Vector2f Pathfinding::gridToWorld(int row, int col, Grid* grid) {
   float gridX = grid->getX();
   float gridY = grid->getY();
   float cellSize = grid->getCellSize();

   // retorna el centro de la celda
   float worldX = gridX + col * cellSize + cellSize / 2.0f;
   float worldY = gridY + row * cellSize + cellSize / 2.0f;

   return sf::Vector2f(worldX, worldY);
}



// reconstruye el camino desde el nodo final hasta el inicial
DynamicArray<sf::Vector2f> Pathfinding::reconstructPath(std::shared_ptr<Node> endNode, Grid* grid) {
   DynamicArray<sf::Vector2f> path;

   std::shared_ptr<Node> current = endNode;
   while (current != nullptr) {
       // convertir coordenadas de grid a mundo
       sf::Vector2f worldPos = gridToWorld(current->row, current->col, grid);
       path.push_back(worldPos);
       current = current->parent;
   }

   // invertir el camino para que vaya del inicio al final
   std::reverse(path.begin(), path.end());

   return path;
}



// implementacion principal del algoritmo a*
DynamicArray<sf::Vector2f> Pathfinding::findPath(Grid* grid, const sf::Vector2f& worldStart, const sf::Vector2f& worldGoal) {
   // convertir coordenadas del mundo a indices del grid
   auto [startRow, startCol] = worldToGrid(worldStart, grid);
   auto [goalRow, goalCol] = worldToGrid(worldGoal, grid);

   // verificar que el inicio y el objetivo son validos
   if (!isCellWalkable(startRow, startCol, grid) || !isCellWalkable(goalRow, goalCol, grid)) {
       // si el inicio o el objetivo no son transitables, intentar encontrar la celda transitable mas cercana
       // para el inicio
       if (!isCellWalkable(startRow, startCol, grid)) {
           bool found = false;
           for (int r = -1; r <= 1 && !found; ++r) {
               for (int c = -1; c <= 1 && !found; ++c) {
                   if (isCellWalkable(startRow + r, startCol + c, grid)) {
                       startRow += r;
                       startCol += c;
                       found = true;
                   }
               }
           }
           if (!found) return {}; // no se puede encontrar inicio valido
       }

       // para el objetivo
       if (!isCellWalkable(goalRow, goalCol, grid)) {
           bool found = false;
           for (int r = -1; r <= 1 && !found; ++r) {
               for (int c = -1; c <= 1 && !found; ++c) {
                   if (isCellWalkable(goalRow + r, goalCol + c, grid)) {
                       goalRow += r;
                       goalCol += c;
                       found = true;
                   }
               }
           }
           if (!found) return {}; // no se puede encontrar objetivo valido
       }
   }

   // priority queue para la lista abierta (min heap basado en f)
   std::priority_queue<std::shared_ptr<Node>,
                      std::vector<std::shared_ptr<Node>>,
                      NodeCompare> openList;

   // set para rastrear nodos en la lista abierta y cerrada
   std::unordered_set<std::string> inOpenList;
   std::unordered_set<std::string> closedList;

   // crear nodo inicial
   auto startNode = std::make_shared<Node>(startRow, startCol);
   startNode->g = 0;
   startNode->h = calculateHeuristic(startRow, startCol, goalRow, goalCol);
   startNode->f = startNode->g + startNode->h;

   openList.push(startNode);
   inOpenList.insert(std::to_string(startRow) + "," + std::to_string(startCol));

   // algoritmo a*
   while (!openList.empty()) {
       // obtener el nodo con menor f
       auto current = openList.top();
       openList.pop();

       std::string currentKey = std::to_string(current->row) + "," + std::to_string(current->col);
       inOpenList.erase(currentKey);

       // verificar si llegamos al objetivo
       if (current->row == goalRow && current->col == goalCol) {
           return reconstructPath(current, grid);
       }

       // agregar a la lista cerrada
       closedList.insert(currentKey);

       // explorar vecinos
       auto neighbors = getNeighbors(current->row, current->col, grid);

       for (const auto& [neighborRow, neighborCol] : neighbors) {
           std::string neighborKey = std::to_string(neighborRow) + "," + std::to_string(neighborCol);

           // si ya esta en la lista cerrada, ignorar
           if (closedList.find(neighborKey) != closedList.end()) {
               continue;
           }

           // calcular el costo g tentativo
           float moveCost = 1.0f;
           float tentativeG = current->g + moveCost;

           // verificar si ya esta en la lista abierta
           bool inOpen = inOpenList.find(neighborKey) != inOpenList.end();

           if (!inOpen || tentativeG < current->g) {
               // crear o actualizar el nodo vecino
               auto neighbor = std::make_shared<Node>(neighborRow, neighborCol);
               neighbor->g = tentativeG;
               neighbor->h = calculateHeuristic(neighborRow, neighborCol, goalRow, goalCol);
               neighbor->f = neighbor->g + neighbor->h;
               neighbor->parent = current;

               if (!inOpen) {
                   openList.push(neighbor);
                   inOpenList.insert(neighborKey);
               }
           }
       }
   }

   // no se encontro camino
   return {};
}



// verifica si existe un camino valido
bool Pathfinding::hasValidPath(Grid* grid, const sf::Vector2f& worldStart, const sf::Vector2f& worldGoal) {
   auto path = findPath(grid, worldStart, worldGoal);
   return !path.empty();
}



// metodos legacy para compatibilidad
sf::Vector2f Pathfinding::moveAlongPath(
   const sf::Vector2f& currentPosition,
   const DynamicArray<sf::Vector2f>& path,
   size_t& currentPathIndex,
   float speed,
   float dt,
   float arrivalThreshold) {

   if (path.empty() || currentPathIndex >= path.size()) {
       return currentPosition;
   }

   sf::Vector2f targetPoint = path[currentPathIndex];
   sf::Vector2f moveDirection = getDirection(currentPosition, targetPoint);
   float distance = getDistance(currentPosition, targetPoint);

   if (distance < arrivalThreshold) {
       currentPathIndex++;

       if (currentPathIndex >= path.size()) {
           return currentPosition;
       }

       targetPoint = path[currentPathIndex];
       moveDirection = getDirection(currentPosition, targetPoint);
   }

   return currentPosition + moveDirection * speed * dt;
}



// verifica si ha llegado al final del camino
bool Pathfinding::hasReachedEnd(size_t currentPathIndex, const DynamicArray<sf::Vector2f>& path) {
   return currentPathIndex >= path.size() - 1;
}



// calcula la direccion normalizada entre dos puntos
sf::Vector2f Pathfinding::getDirection(const sf::Vector2f& from, const sf::Vector2f& to) {
   sf::Vector2f direction = to - from;
   float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

   if (length > 0.0001f) {
       direction /= length;
   }

   return direction;
}



// calcula la distancia entre dos puntos
float Pathfinding::getDistance(const sf::Vector2f& from, const sf::Vector2f& to) {
   sf::Vector2f delta = to - from;
   return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}