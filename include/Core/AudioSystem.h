#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class AudioSystem {
private:
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::vector<std::unique_ptr<sf::Sound>> activeSounds;
    std::vector<std::unique_ptr<sf::Music>> activeMusic;
    std::vector<std::unique_ptr<sf::Sound>> persistentSounds;
    float calculateFinalSfxVolume() const;
    float calculateFinalMusicVolume() const;
    void cleanupPersistentSounds();

public:
    AudioSystem();
    ~AudioSystem();

    bool loadSound(const std::string& id, const std::string& filepath);
    void playSound(const std::string& id);
    bool playMusic(const std::string& filepath, bool loop = false);
    void stopAllMusic();
    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void setMusicVolume(float volume);
    float getMusicVolume() const;
    void setSfxVolume(float volume);
    float getSfxVolume() const;
    void updateAllVolumes();
    void update();
    bool isMusicPlaying() const;
    bool isMusicPlaying(const std::string& filepath) const;
    bool playMusicIfNotPlaying(const std::string& filepath, bool loop = false);
    std::string getCurrentMusicFilepath() const;
    std::string currentMusicPath;
    void playSoundPersistent(const std::string& id);
};