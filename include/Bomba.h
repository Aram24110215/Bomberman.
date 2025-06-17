#pragma once
#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include <iostream> // Para manejo de errores

class Bomba {
public:
    sf::Vector2i gridPos; // Posición de la bomba en la cuadrícula
    sf::Clock timer; // Reloj para controlar el tiempo de explosión
    sf::Clock animationClock; // Reloj para controlar la animación
    bool explotada = false;
    bool enExpansion = false; // Indica si la bomba está en expansión
    bool terminada = false; // Indica si la explosión terminó
    bool esJugador1;  // Nueva variable para rastrear qué jugador colocó la bomba

    Bomba(sf::Vector2i pos, bool esJugador1 = true) 
        : gridPos(pos), esJugador1(esJugador1) {
        // Define the transparency color once
        const sf::Color transparentColor(112, 234, 122); // #70EA7A

        sf::Image bombaImage, centerImage, secondImage, lastImage;

        // Load and process bomb texture
        if (!bombaImage.loadFromFile("assets/images/bomba.png")) {
            std::cerr << "Error: No se pudo cargar la textura de la bomba." << std::endl;
        }
        bombaImage.createMaskFromColor(transparentColor);
        bombaTexture.loadFromImage(bombaImage);

        // Load and process center explosion texture
        if (!centerImage.loadFromFile("assets/images/explosion_center.png")) {
            std::cerr << "Error: No se pudo cargar la textura del centro de la explosión." << std::endl;
        }
        centerImage.createMaskFromColor(transparentColor);
        centerTexture.loadFromImage(centerImage);

        // Load and process second explosion texture
        if (!secondImage.loadFromFile("assets/images/explosion_second.png")) {
            std::cerr << "Error: No se pudo cargar la textura del segundo bloque de la explosión." << std::endl;
        }
        secondImage.createMaskFromColor(transparentColor);
        secondTexture.loadFromImage(secondImage);

        // Load and process last explosion texture
        if (!lastImage.loadFromFile("assets/images/explosion_last.png")) {
            std::cerr << "Error: No se pudo cargar la textura del último bloque de la explosión." << std::endl;
        }
        lastImage.createMaskFromColor(transparentColor);
        lastTexture.loadFromImage(lastImage);

        // Configure sprites with the processed textures
        bombaSprite.setTexture(bombaTexture);
        bombaSprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight)); // Primer frame
        bombaSprite.setPosition(gridPos.x * Mapa::tile, gridPos.y * Mapa::tile); // Centrar en el bloque

        // Configurar los sprites de la explosión
        centerSprite.setTexture(centerTexture);
        secondSprite.setTexture(secondTexture);
        lastSprite.setTexture(lastTexture);

        // Configurar el tamaño inicial de los rectángulos de textura
        centerSprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        secondSprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        lastSprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    }

    void update(Mapa& mapa, Personaje& jugador1, Personaje& jugador2) {
        // Actualizar la animación de la bomba
        if (!explotada && animationClock.getElapsedTime().asSeconds() >= frameTime) {
            currentFrame = (currentFrame + 1) % numFrames; // Cambiar al siguiente frame
            bombaSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            animationClock.restart();
        }

        // Verificar si la bomba debe estallar
        if (timer.getElapsedTime().asSeconds() >= explosionTime && !explotada) {
            explotar(mapa, jugador1, jugador2); // Pasar los jugadores como argumentos
        }

        // Actualizar la animación de la explosión
        if (enExpansion && animationClock.getElapsedTime().asSeconds() >= frameTime) {
            currentFrame = (currentFrame + 1) % numFramesExplosion; // Cambiar al siguiente frame
            
            // Actualizar los frames de animación
            centerSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            secondSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            lastSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            
            animationClock.restart();

            // Marcar la bomba como terminada cuando la animación se completa
            if (currentFrame == numFramesExplosion - 1) {
                enExpansion = false;
                terminada = true; // Marcar la explosión como terminada
            }
        }
    }

    void explotar(Mapa& mapa, Personaje& jugador1, Personaje& jugador2) {
        // Destruir el bloque donde está la bomba y los bloques adyacentes
        if (mapa.grid[gridPos.y][gridPos.x] == 2) {
            mapa.destruirBloque(gridPos.x, gridPos.y);
        }

        for (int i = 1; i <= 2; ++i) { // Onda de expansión de 2 bloques
            // Derecha
            if (gridPos.x + i < Mapa::ancho && !mapa.esSolido(gridPos.x + i, gridPos.y)) {
                mapa.destruirBloque(gridPos.x + i, gridPos.y);
                if (sf::Vector2i(gridPos.x + i, gridPos.y) == sf::Vector2i(static_cast<int>(jugador1.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador1.getPosition().y / Mapa::tile))) {
                    jugador1 = Personaje(sf::Vector2f(Mapa::tile, Mapa::tile)); // Reiniciar jugador 1
                }
                if (sf::Vector2i(gridPos.x + i, gridPos.y) == sf::Vector2i(static_cast<int>(jugador2.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador2.getPosition().y / Mapa::tile))) {
                    jugador2 = Personaje(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile)); // Reiniciar jugador 2
                }
            }

            // Izquierda
            if (gridPos.x - i >= 0 && !mapa.esSolido(gridPos.x - i, gridPos.y)) {
                mapa.destruirBloque(gridPos.x - i, gridPos.y);
                if (sf::Vector2i(gridPos.x - i, gridPos.y) == sf::Vector2i(static_cast<int>(jugador1.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador1.getPosition().y / Mapa::tile))) {
                    jugador1 = Personaje(sf::Vector2f(Mapa::tile, Mapa::tile)); // Reiniciar jugador 1
                }
                if (sf::Vector2i(gridPos.x - i, gridPos.y) == sf::Vector2i(static_cast<int>(jugador2.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador2.getPosition().y / Mapa::tile))) {
                    jugador2 = Personaje(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile)); // Reiniciar jugador 2
                }
            }

            // Abajo
            if (gridPos.y + i < Mapa::alto && !mapa.esSolido(gridPos.x, gridPos.y + i)) {
                mapa.destruirBloque(gridPos.x, gridPos.y + i);
                if (sf::Vector2i(gridPos.x, gridPos.y + i) == sf::Vector2i(static_cast<int>(jugador1.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador1.getPosition().y / Mapa::tile))) {
                    jugador1 = Personaje(sf::Vector2f(Mapa::tile, Mapa::tile)); // Reiniciar jugador 1
                }
                if (sf::Vector2i(gridPos.x, gridPos.y + i) == sf::Vector2i(static_cast<int>(jugador2.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador2.getPosition().y / Mapa::tile))) {
                    jugador2 = Personaje(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile)); // Reiniciar jugador 2
                }
            }

            // Arriba
            if (gridPos.y - i >= 0 && !mapa.esSolido(gridPos.x, gridPos.y - i)) {
                mapa.destruirBloque(gridPos.x, gridPos.y - i);
                if (sf::Vector2i(gridPos.x, gridPos.y - i) == sf::Vector2i(static_cast<int>(jugador1.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador1.getPosition().y / Mapa::tile))) {
                    jugador1 = Personaje(sf::Vector2f(Mapa::tile, Mapa::tile)); // Reiniciar jugador 1
                }
                if (sf::Vector2i(gridPos.x, gridPos.y - i) == sf::Vector2i(static_cast<int>(jugador2.getPosition().x / Mapa::tile),
                                                                           static_cast<int>(jugador2.getPosition().y / Mapa::tile))) {
                    jugador2 = Personaje(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile)); // Reiniciar jugador 2
                }
            }
        }
        explotada = true; // Marcar la bomba como explotada
        enExpansion = true; // Activar la expansión
    }

    void draw(sf::RenderWindow& window, Mapa& mapa) {
        if (!explotada) {
            bombaSprite.setPosition(gridPos.x * Mapa::tile, gridPos.y * Mapa::tile); // Centrar la bomba en el bloque
            window.draw(bombaSprite);
        } else if (enExpansion) {
            // Configurar el origen del sprite para que esté centrado
            centerSprite.setOrigin(frameWidth / 2, frameHeight / 2);
            secondSprite.setOrigin(frameWidth / 2, frameHeight / 2);
            lastSprite.setOrigin(frameWidth / 2, frameHeight / 2);

            // Dibujar el centro de la explosión
            if (!mapa.esSolido(gridPos.x, gridPos.y)) {
                centerSprite.setPosition(gridPos.x * Mapa::tile + Mapa::tile / 2, gridPos.y * Mapa::tile + Mapa::tile / 2);
                window.draw(centerSprite);
            }

            // Dibujar el segundo bloque de la explosión
            for (int i = 1; i <= 1; ++i) {
                // Derecha
                if (gridPos.x + i < Mapa::ancho && !mapa.esSolido(gridPos.x + i, gridPos.y)) {
                    secondSprite.setRotation(0); // Sin rotación
                    secondSprite.setPosition((gridPos.x + i) * Mapa::tile + Mapa::tile / 2, gridPos.y * Mapa::tile + Mapa::tile / 2);
                    window.draw(secondSprite);
                }

                // Izquierda
                if (gridPos.x - i >= 0 && !mapa.esSolido(gridPos.x - i, gridPos.y)) {
                    secondSprite.setRotation(180); // Rotar hacia la izquierda
                    secondSprite.setPosition((gridPos.x - i) * Mapa::tile + Mapa::tile / 2, gridPos.y * Mapa::tile + Mapa::tile / 2);
                    window.draw(secondSprite);
                }

                // Abajo
                if (gridPos.y + i < Mapa::alto && !mapa.esSolido(gridPos.x, gridPos.y + i)) {
                    secondSprite.setRotation(90); // Rotar hacia abajo
                    secondSprite.setPosition(gridPos.x * Mapa::tile + Mapa::tile / 2, (gridPos.y + i) * Mapa::tile + Mapa::tile / 2);
                    window.draw(secondSprite);
                }

                // Arriba
                if (gridPos.y - i >= 0 && !mapa.esSolido(gridPos.x, gridPos.y - i)) {
                    secondSprite.setRotation(270); // Rotar hacia arriba
                    secondSprite.setPosition(gridPos.x * Mapa::tile + Mapa::tile / 2, (gridPos.y - i) * Mapa::tile + Mapa::tile / 2);
                    window.draw(secondSprite);
                }
            }

            // Dibujar el último bloque de la explosión
            for (int i = 2; i <= 2; ++i) {
                // Derecha
                if (gridPos.x + i < Mapa::ancho && !mapa.esSolido(gridPos.x + i, gridPos.y)) {
                    lastSprite.setRotation(0); // Sin rotación
                    lastSprite.setPosition((gridPos.x + i) * Mapa::tile + Mapa::tile / 2, gridPos.y * Mapa::tile + Mapa::tile / 2);
                    window.draw(lastSprite);
                }

                // Izquierda
                if (gridPos.x - i >= 0 && !mapa.esSolido(gridPos.x - i, gridPos.y)) {
                    lastSprite.setRotation(180); // Rotar hacia la izquierda
                    lastSprite.setPosition((gridPos.x - i) * Mapa::tile + Mapa::tile / 2, gridPos.y * Mapa::tile + Mapa::tile / 2);
                    window.draw(lastSprite);
                }

                // Abajo
                if (gridPos.y + i < Mapa::alto && !mapa.esSolido(gridPos.x, gridPos.y + i)) {
                    lastSprite.setRotation(90); // Rotar hacia abajo
                    lastSprite.setPosition(gridPos.x * Mapa::tile + Mapa::tile / 2, (gridPos.y + i) * Mapa::tile + Mapa::tile / 2);
                    window.draw(lastSprite);
                }

                // Arriba
                if (gridPos.y - i >= 0 && !mapa.esSolido(gridPos.x, gridPos.y - i)) {
                    lastSprite.setRotation(270); // Rotar hacia arriba
                    lastSprite.setPosition(gridPos.x * Mapa::tile + Mapa::tile / 2, (gridPos.y - i) * Mapa::tile + Mapa::tile / 2);
                    window.draw(lastSprite);
                }
            }
        }
    }

private:
    sf::Sprite bombaSprite; // Sprite para la bomba
    sf::Sprite centerSprite; // Sprite para el centro de la explosión
    sf::Sprite secondSprite; // Sprite para el segundo bloque de la explosión
    sf::Sprite lastSprite;   // Sprite para el último bloque de la explosión

    sf::Texture bombaTexture; // Textura para la bomba
    sf::Texture centerTexture; // Textura para el centro de la explosión
    sf::Texture secondTexture; // Textura para el segundo bloque de la explosión
    sf::Texture lastTexture;   // Textura para el último bloque de la explosión

    int frameWidth = 16; // Ancho de cada frame
    int frameHeight = 16; // Alto de cada frame
    int numFrames = 3; // Número total de frames en la animación de la bomba
    int numFramesExplosion = 3; // Número total de frames en la animación de la explosión
    int currentFrame = 0; // Frame actual
    float frameTime = 1.0f; // Tiempo entre cada frame en segundos (1 segundo por frame)
    float explosionTime = 3.0f; // Tiempo antes de que la bomba explote (en segundos)
};
