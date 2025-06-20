#pragma once
#include <SFML/Audio.hpp>
#include <stdexcept>

class AudioManager {
public:
    static AudioManager& GetInstance() {
        static AudioManager instance;
        return instance;
    }

    void CargarSonidos() {
        if (!explosionBuffer.loadFromFile("assets/sounds/explosion.wav"))
            throw std::runtime_error("Error cargando sonido de explosión");
        if (!powerupBuffer.loadFromFile("assets/sounds/powerup.wav"))
            throw std::runtime_error("Error cargando sonido de powerup");
        if (!musicaBuffer.loadFromFile("assets/sounds/theme.wav"))
            throw std::runtime_error("Error cargando música de fondo");
        
        explosion.setBuffer(explosionBuffer);
        powerup.setBuffer(powerupBuffer);
        musica.setBuffer(musicaBuffer);
        musica.setLoop(true);
    }

    void PlayExplosion() { explosion.play(); }
    void PlayPowerup() { powerup.play(); }
    void PlayMusica() { musica.play(); }
    void StopMusica() { musica.stop(); }

private:
    AudioManager() { CargarSonidos(); }
    sf::SoundBuffer explosionBuffer, powerupBuffer, musicaBuffer;
    sf::Sound explosion, powerup, musica;
};