cmake_minimum_required(VERSION 3.10)
project(Genetic_Kingdom)

set(CMAKE_CXX_STANDARD 17)

# Encuentra los paquetes de SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Incluye directorios
include_directories(include)

# Agrega los archivos fuente
set(SOURCES
        src/main.cpp
        src/Core/Game.cpp
        src/Core/AudioSystem.cpp
        src/UI/Button.cpp
        src/States/MenuStates/MenuState.cpp
        src/States/GameplayState.cpp
        src/States/PauseState.cpp
        src/Effects/Particle.cpp
        src/Effects/ParticleSystem.cpp
        src/States/MenuStates/SettingsState.cpp
        src/States/MenuStates/ManualState.cpp
        src/States/MenuStates/CreditsState.cpp
        src/UI/Slider.cpp
        src/Game/Grid/Grid.cpp
        src/Game/Grid/Cell.cpp
        src/Game/Enemies/Enemy.cpp
        src/Game/Enemies/Ogre.cpp
        src/Game/Systems/WaveManager.cpp
        src/Game/Systems/Pathfinding.cpp
        src/Game/Genetics/Chromosome.cpp
        src/Game/Genetics/Genetics.cpp
        include/Game/Towers/Tower.h
        include/Game/Towers/Mage.h
        include/Game/Towers/Archer.h
        include/Game/Towers/Gunner.h
        src/Game/Towers/Archer.cpp
        src/Game/Towers/Gunner.cpp
        src/Game/Towers/Mage.cpp
)

# Crea el ejecutable
add_executable(Genetic_Kingdom ${SOURCES})

# Enlaza las bibliotecas de SFML
target_link_libraries(Genetic_Kingdom sfml-graphics sfml-window sfml-system sfml-audio)

# Copia la carpeta assets al directorio donde está el ejecutable
add_custom_command(TARGET Genetic_Kingdom POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Genetic_Kingdom>/assets)
