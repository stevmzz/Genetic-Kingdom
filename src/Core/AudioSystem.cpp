#include "../include/Core/AudioSystem.h"
#include <iostream>
#include <algorithm>

// constructor del sistema de audio
AudioSystem::AudioSystem()
    : masterVolume(50.0f),
      musicVolume(40.0f),
      sfxVolume(60.0f),
      currentMusicPath("") {
}



// destructor del sistema de audio
AudioSystem::~AudioSystem() {
    stopAllMusic();
    activeSounds.clear();
    soundBuffers.clear();
}



// calcula el volumen final de los efectos de sonido
float AudioSystem::calculateFinalSfxVolume() const {
    return (masterVolume * sfxVolume) / 100.0f;
}



// calcula el volumen final de la musica
float AudioSystem::calculateFinalMusicVolume() const {
    return (masterVolume * musicVolume) / 100.0f;
}



// carga un sonido en el sistema
bool AudioSystem::loadSound(const std::string& id, const std::string& filepath) {
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filepath)) {
        std::cerr << "error cargando sonido: " << filepath << std::endl;
        return false;
    }

    soundBuffers[id] = std::move(buffer);
    return true;
}



// reproduce un sonido por su identificador
void AudioSystem::playSound(const std::string& id) {
    auto it = soundBuffers.find(id);
    if (it == soundBuffers.end()) {
        std::cerr << "sonido no encontrado: " << id << std::endl;
        return;
    }

    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(*it->second);
    sound->setVolume(calculateFinalSfxVolume());
    sound->play();

    activeSounds.push_back(std::move(sound));
}



// verifica si hay alguna musica reproduciendose
bool AudioSystem::isMusicPlaying() const {
    for (const auto& music : activeMusic) {
        if (music->getStatus() == sf::Music::Playing) {
            return true;
        }
    }
    return false;
}



// verifica si una musica especifica esta reproduciendose
bool AudioSystem::isMusicPlaying(const std::string& filepath) const {
    return false;
}



// obtiene la ruta del archivo de musica actual
std::string AudioSystem::getCurrentMusicFilepath() const {
    return currentMusicPath;
}



// reproduce una musica solo si no se esta reproduciendo
bool AudioSystem::playMusicIfNotPlaying(const std::string& filepath, bool loop) {
    // si ya estamos reproduciendo esta musica, no hacemos nada
    if (isMusicPlaying() && filepath == currentMusicPath) {
        return true;
    }

    // si no hay musica o es diferente, reproducimos la nueva
    return playMusic(filepath, loop);
}



// reproduce una musica
bool AudioSystem::playMusic(const std::string& filepath, bool loop) {
    // primero, detener cualquier musica que este sonando
    stopAllMusic();

    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(filepath)) {
        std::cerr << "Error cargando música: " << filepath << std::endl;
        return false;
    }

    // guardar la ruta de la musica actual
    currentMusicPath = filepath;

    music->setLoop(loop);
    music->setVolume(calculateFinalMusicVolume());
    music->play();

    activeMusic.push_back(std::move(music));
    return true;
}



// detiene toda la musica
void AudioSystem::stopAllMusic() {
    for (auto& music : activeMusic) {
        if (music->getStatus() == sf::Music::Playing) {
            music->stop();
        }
    }
    activeMusic.clear();
    currentMusicPath = ""; // limpiar la ruta actual
}



// establece el volumen maestro
void AudioSystem::setMasterVolume(float volume) {
    // limitar el volumen entre 0 y 100
    masterVolume = std::max(0.0f, std::min(100.0f, volume));
    updateAllVolumes();
}



// obtiene el volumen maestro
float AudioSystem::getMasterVolume() const {
    return masterVolume;
}



// establece el volumen de la musica
void AudioSystem::setMusicVolume(float volume) {
    // limitar el volumen entre 0 y 100
    musicVolume = std::max(0.0f, std::min(100.0f, volume));

    // actualizar el volumen de toda la musica activa
    float finalVolume = calculateFinalMusicVolume();
    for (auto& music : activeMusic) {
        if (music->getStatus() == sf::Music::Playing) {
            music->setVolume(finalVolume);
        }
    }
}



// obtiene el volumen de la musica
float AudioSystem::getMusicVolume() const {
    return musicVolume;
}



// establece el volumen de los efectos de sonido
void AudioSystem::setSfxVolume(float volume) {
    // limitar el volumen entre 0 y 100
    sfxVolume = std::max(0.0f, std::min(100.0f, volume));

    // actualizar el volumen de todos los sonidos activos
    float finalVolume = calculateFinalSfxVolume();
    for (auto& sound : activeSounds) {
        if (sound->getStatus() == sf::Sound::Playing) {
            sound->setVolume(finalVolume);
        }
    }
}



// obtiene el volumen de los efectos de sonido
float AudioSystem::getSfxVolume() const {
    return sfxVolume;
}



// actualiza todos los volumenes
void AudioSystem::updateAllVolumes() {
    // actualizar el volumen de todos los sonidos activos
    float finalSfxVolume = calculateFinalSfxVolume();
    for (auto& sound : activeSounds) {
        if (sound->getStatus() == sf::Sound::Playing) {
            sound->setVolume(finalSfxVolume);
        }
    }

    // actualizar el volumen de toda la musica activa
    float finalMusicVolume = calculateFinalMusicVolume();
    for (auto& music : activeMusic) {
        if (music->getStatus() == sf::Music::Playing) {
            music->setVolume(finalMusicVolume);
        }
    }
}



// elimina los sonidos que ya no estan reproduciendose
void AudioSystem::update() {
    // eliminar sonidos que ya no estan en reproduccion
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](const std::unique_ptr<sf::Sound>& sound) {
                return sound->getStatus() != sf::Sound::Playing;
            }),
        activeSounds.end()
    );
}