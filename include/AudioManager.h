#pragma once
#include <SFML/Audio.hpp>
#include <stdexcept>

class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    void cargarSonidos() {
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

    void playExplosion() { explosion.play(); }
    void playPowerup() { powerup.play(); }
    void playMusica() { musica.play(); }
    void stopMusica() { musica.stop(); }

private:
    AudioManager() { cargarSonidos(); }
    sf::SoundBuffer explosionBuffer, powerupBuffer, musicaBuffer;
    sf::Sound explosion, powerup, musica;
};