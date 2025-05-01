#include "../include/Core/Game.h"
#include "../include/States/MenuStates/MenuState.h"
#include <iostream>



// constructor de la clase game, inicializa la variable running en falso
Game::Game() : running(false) {
}



// destructor de la clase game, libera todos los estados de la pila
Game::~Game() {
    while (!states.empty()) {
        states.pop();
    }
}



// metodo para inicializar la ventana y el estado inicial del juego
void Game::init() {
    window.create(sf::VideoMode::getDesktopMode(), "Genetic Kingdom", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    // carga la fuente desde un archivo
    if (!font.loadFromFile("assets/fonts/PressStart2P.ttf")) {
        std::cerr << "Error cargando la fuente" << std::endl;
    }

    // crea el estado del menu y lo agrega a la pila
    auto menuState = std::make_shared<MenuState>();
    pushState(menuState);

    running = true;
}



// bucle principal del juego
void Game::run() {
    sf::Time frameTime;

    // mientras el juego este corriendo, la ventana abierta y haya estados en la pila
    while (running && window.isOpen() && !states.empty()) {
        frameTime = clock.restart();
        float dt = frameTime.asSeconds();

        sf::Event event;
        // procesa todos los eventos de la ventana
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // delega el manejo de eventos al estado actual
            if (!states.empty())
                states.top()->handleEvents(event);
        }

        // actualiza el estado actual
        if (!states.empty())
            states.top()->update(dt);

        // actualiza el sistema de audio (elimina sonidos terminados)
        audioSystem.update();

        // limpia la pantalla con un color gris oscuro
        window.clear(sf::Color(25, 25, 25));

        // renderiza el estado actual
        if (!states.empty())
            states.top()->render(window);

        // muestra lo renderizado en pantalla
        window.display();
    }
}



// agrega un nuevo estado a la pila y lo inicializa
void Game::pushState(std::shared_ptr<GameState> state) {
    state->setGame(this);
    states.push(state);
    state->init();
}



// elimina el estado actual de la pila y llama a su metodo cleanup
void Game::popState() {
    if (!states.empty()) {
        states.top()->cleanup();
        states.pop();
    }
}



// cambia el estado actual por uno nuevo
void Game::changeState(std::shared_ptr<GameState> state) {
    if (!states.empty()) {
        popState();
    }
    pushState(state);
}



// obtiene el estado actual desde la cima de la pila
std::shared_ptr<GameState> Game::getCurrentState() {
    if (!states.empty()) {
        return states.top();
    }
    return nullptr;
}