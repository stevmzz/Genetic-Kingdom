#include "../include/Game/Enemies/Enemy.h"
#include "../include/Game/Systems/Pathfinding.h"
#include "../include/Game/Grid/Grid.h"
#include <cmath>
#include <iostream>
#include "Game/Towers/Tower.h"

// fuente compartida para mostrar textos flotantes de daño
sf::Font Enemy::sharedFont;

// constructor del enemigo con parametros individuales
Enemy::Enemy(float health, float speed, float arrowRes, float magicRes, float artilleryRes, int goldReward, const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path)
    :   id(-1),
        health(health),
        maxHealth(health),
        speed(speed),
        arrowResistance(arrowRes),
        magicResistance(magicRes),
        artilleryResistance(artilleryRes),
        goldReward(goldReward),
        position(position),
        isActive(true),
        path(path),
        currentPathIndex(0),
        totalDistanceTraveled(0.0f),
        totalDamageReceived(0.0f) {

    // calcular direccion inicial hacia el primer punto del camino
    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex + 1]);
    }

    // iniciar medicion del tiempo de vida
    lifeTimer.restart();
}



// constructor del enemigo basado en cromosoma genetico
Enemy::Enemy(const Chromosome& chromosome, int goldReward, const sf::Vector2f& position, const DynamicArray<sf::Vector2f>& path)
    :   id(-1),
        health(chromosome.getHealth()),
        maxHealth(chromosome.getHealth()),
        speed(chromosome.getSpeed()),
        arrowResistance(chromosome.getArrowResistance()),
        magicResistance(chromosome.getMagicResistance()),
        artilleryResistance(chromosome.getArtilleryResistance()),
        goldReward(goldReward),
        position(position),
        isActive(true),
        path(path),
        currentPathIndex(0),
        totalDistanceTraveled(0.0f),
        totalDamageReceived(0.0f) {

    // calcular direccion inicial hacia el primer punto del camino
    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex + 1]);
    }

    // iniciar medicion del tiempo de vida
    lifeTimer.restart();
}



// carga la textura del archivo y configura el sprite
bool Enemy::loadTexture(const std::string& filename) {
    if (!texture.loadFromFile(filename)) {
        return false;
    }

    sprite.setTexture(texture);

    // centrar el origen del sprite para rotaciones correctas
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2, bounds.height / 2);
    sprite.setPosition(position);

    return true;
}



// verifica si el enemigo sigue con vida y activo
bool Enemy::isAlive() const {
    return health > 0.f && isActive;
}



// verifica si el enemigo alcanzo el final del camino
bool Enemy::hasReachedEnd() const {
    return Pathfinding::hasReachedEnd(currentPathIndex, path);
}



// obtiene la cantidad de oro que da al morir
int Enemy::getGoldReward() const {
    return goldReward;
}



// obtiene la posicion actual del enemigo en el mapa
sf::Vector2f Enemy::getPosition() const {
    return position;
}



// dibuja el enemigo y sus elementos visuales en pantalla
void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (isActive) {
        target.draw(sprite, states);

        // dibujar barra de vida con color segun porcentaje de salud
        sf::RectangleShape healthBar;
        float healthPercentage = health / maxHealth;
        healthBar.setSize(sf::Vector2f(40.f * healthPercentage, 5.f));

        // cambiar color segun porcentaje de vida
        if (healthPercentage > 0.6f) {
            healthBar.setFillColor(sf::Color::Green);
        } else if (healthPercentage > 0.3f) {
            healthBar.setFillColor(sf::Color::Yellow);
        } else {
            healthBar.setFillColor(sf::Color::Red);
        }

        healthBar.setPosition(position.x - 20.f, position.y - 40.f);
        target.draw(healthBar, states);

        // dibujar borde negro alrededor de la barra de vida
        sf::RectangleShape healthBarBorder;
        healthBarBorder.setSize(sf::Vector2f(40.f, 5.f));
        healthBarBorder.setFillColor(sf::Color::Transparent);
        healthBarBorder.setOutlineColor(sf::Color::Black);
        healthBarBorder.setOutlineThickness(1.f);
        healthBarBorder.setPosition(position.x - 20.f, position.y - 40.f);
        target.draw(healthBarBorder, states);
    }

    // dibujar numeros flotantes que muestran el daño recibido
    for (const auto& dmgText : floatingTexts) {
        target.draw(dmgText.text, states);
    }
}



// asigna un nuevo camino para que siga el enemigo
void Enemy::setPath(const DynamicArray<sf::Vector2f>& newPath) {
    path = newPath;
    currentPathIndex = 0;

    // recalcular direccion hacia el primer punto del nuevo camino
    if (!path.empty() && currentPathIndex < path.size() - 1) {
        direction = Pathfinding::getDirection(position, path[currentPathIndex]);
    }
}



// recalcula el camino usando algoritmo a* para evitar obstaculos
void Enemy::recalculatePath(Grid* grid, const sf::Vector2f& goal) {
    if (!grid) {
        return;
    }

    // buscar nueva ruta desde posicion actual hasta el objetivo
    auto newPath = Pathfinding::findPath(grid, position, goal);

    if (!newPath.empty()) {
        // actualizar camino y reiniciar indice
        path = newPath;
        currentPathIndex = 0;

        // calcular nueva direccion hacia el primer punto
        if (path.size() > 1) {
            direction = Pathfinding::getDirection(position, path[0]);
        }
    }
}



// asigna un identificador unico al enemigo
void Enemy::setId(int id) {
    this->id = id;
}



// obtiene el identificador unico del enemigo
int Enemy::getId() const {
    return id;
}



// obtiene la distancia total que ha recorrido el enemigo
float Enemy::getTotalDistanceTraveled() const {
    return totalDistanceTraveled;
}



// obtiene el daño total acumulado que ha recibido
float Enemy::getTotalDamageReceived() const {
    return totalDamageReceived;
}



// obtiene cuantos segundos ha estado vivo el enemigo
float Enemy::getTimeAlive() const {
    return lifeTimer.getElapsedTime().asSeconds();
}



// calcula que tan efectivo ha sido el daño aplicado
float Enemy::getDamageEffectiveness() const {
    if (maxHealth <= 0.0f) return 0.0f;
    return std::min(totalDamageReceived / maxHealth, 2.0f); // limite maximo del 200%
}



// aplica daño directo al enemigo y actualiza estadisticas
void Enemy::receiveDamage(float damage) {
    // registrar el daño para estadisticas
    trackDamage(damage);

    // reducir puntos de vida
    health -= damage;

    // mostrar numero flotante con el daño
    createDamageText(damage);

    // marcar como muerto si la vida llega a cero
    if (health <= 0.f) {
        health = 0.f;
        isActive = false;
    }
}



// aplica daño considerando resistencias segun el tipo de ataque
void Enemy::takeDamage(float amount, const std::string& damageType) {
    float damageMultiplier = 1.0f;

    // calcular multiplicador segun resistencias del enemigo
    if (damageType == "arrow") {
        damageMultiplier = 1.0f - arrowResistance;
    } else if (damageType == "magic") {
        damageMultiplier = magicResistance;
    } else if (damageType == "artillery") {
        damageMultiplier = artilleryResistance;
    }

    // evitar daño negativo por resistencias muy altas
    damageMultiplier = std::max(0.0f, damageMultiplier);

    float finalDamage = amount * damageMultiplier;

    // aplicar el daño calculado
    receiveDamage(finalDamage);
}



// actualiza el estado del enemigo en cada frame
void Enemy::update(float dt) {
    // actualizar animaciones de textos flotantes
    updateFloatingTexts(dt);

    // mover al enemigo si esta vivo y tiene camino
    if (isActive && !path.empty() && currentPathIndex < path.size()) {
        updateMovement(dt);
    }
}



// registra el daño recibido para estadisticas y debug
void Enemy::trackDamage(float damage) {
    totalDamageReceived += damage;

    // mensaje de debug para seguimiento del daño
    if (damage > 0) {
        std::cout << "Enemigo " << id << " recibió " << damage
                  << " de daño. Total acumulado: " << totalDamageReceived << "\n";
    }
}



// crea un texto flotante que muestra el daño recibido
void Enemy::createDamageText(float damage) {
    FloatingDamageText damageText;
    damageText.text.setFont(sharedFont);
    damageText.text.setCharacterSize(16);
    damageText.text.setFillColor(sf::Color::Red);
    damageText.text.setString("-" + std::to_string(static_cast<int>(damage)));

    // centrar el texto
    sf::FloatRect bounds = damageText.text.getLocalBounds();
    damageText.text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // posicionar encima del enemigo con variacion aleatoria
    float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
    damageText.text.setPosition(position.x + offsetX, position.y - 60.f);

    floatingTexts.push_back(damageText);
}



// actualiza la animacion de los textos de daño flotantes
void Enemy::updateFloatingTexts(float dt) {
    for (size_t i = 0; i < floatingTexts.size(); ) {
        float t = floatingTexts[i].timer.getElapsedTime().asSeconds();

        // eliminar texto despues de 0.8 segundos
        if (t > 0.8f) {
            floatingTexts.erase(i);
        } else {
            // mover el texto hacia arriba
            sf::Vector2f pos = floatingTexts[i].text.getPosition();
            pos.y -= 20.f * dt;
            floatingTexts[i].text.setPosition(pos);

            // aplicar efecto de desvanecimiento gradual
            sf::Color color = floatingTexts[i].text.getFillColor();
            color.a = static_cast<sf::Uint8>(255 * (1.0f - t / 0.8f));
            floatingTexts[i].text.setFillColor(color);

            ++i;
        }
    }
}



// actualiza la posicion y movimiento del enemigo
void Enemy::updateMovement(float dt) {
    sf::Vector2f oldPosition = position;
    position = Pathfinding::moveAlongPath(position, path, currentPathIndex, speed, dt);
    sprite.setPosition(position);

    // calcular distancia recorrida en este frame para estadisticas
    sf::Vector2f deltaPos = position - oldPosition;
    float distance = std::sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
    totalDistanceTraveled += distance;

    // actualizar direccion hacia el siguiente punto del camino
    if (currentPathIndex < path.size()) {
        sf::Vector2f nextPoint = path[currentPathIndex];
        sf::Vector2f moveDirection = Pathfinding::getDirection(position, nextPoint);
        direction = moveDirection;
    }
}



// establece la fuente que usaran todos los enemigos para textos
void Enemy::setSharedFont(const sf::Font& font) {
    sharedFont = font;
}