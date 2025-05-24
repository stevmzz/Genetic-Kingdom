#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <queue>
#include <unordered_set>
#include <memory>

class Grid;

class Pathfinding {
private:
    struct Node {
        int row, col;
        float g;
        float h;
        float f;
        std::shared_ptr<Node> parent;

        Node(int r, int c) : row(r), col(c), g(0), h(0), f(0), parent(nullptr) {}

        bool operator==(const Node& other) const {
            return row == other.row && col == other.col;
        }
    };

    struct NodeHash {
        std::size_t operator()(const Node& node) const {
            return std::hash<int>()(node.row) ^ (std::hash<int>()(node.col) << 1);
        }
    };

    struct NodeCompare {
        bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
            return a->f > b->f;
        }
    };

    static float calculateHeuristic(int r1, int c1, int r2, int c2);
    static std::vector<std::pair<int, int>> getNeighbors(int row, int col, Grid* grid);
    static std::vector<sf::Vector2f> reconstructPath(std::shared_ptr<Node> endNode, Grid* grid);
    static bool isValidCell(int row, int col, Grid* grid);
    static bool isCellWalkable(int row, int col, Grid* grid);

public:
    static std::vector<sf::Vector2f> findPath(
        Grid* grid,
        const sf::Vector2f& worldStart,
        const sf::Vector2f& worldGoal
    );

    static bool hasValidPath(
        Grid* grid,
        const sf::Vector2f& worldStart,
        const sf::Vector2f& worldGoal
    );

    static std::pair<int, int> worldToGrid(
        const sf::Vector2f& worldPos,
        Grid* grid
    );

    static sf::Vector2f gridToWorld(
        int row, int col,
        Grid* grid
    );

    static sf::Vector2f moveAlongPath(
        const sf::Vector2f& currentPosition,
        const std::vector<sf::Vector2f>& path,
        size_t& currentPathIndex,
        float speed,
        float dt,
        float arrivalThreshold = 5.0f
    );

    static bool hasReachedEnd(size_t currentPathIndex, const std::vector<sf::Vector2f>& path);
    static sf::Vector2f getDirection(const sf::Vector2f& from, const sf::Vector2f& to);
    static float getDistance(const sf::Vector2f& from, const sf::Vector2f& to);
};