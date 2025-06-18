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
    bool hitEnemigo;
    Bomba(sf::Vector2i pos, bool esJugador1 = true) 
        : gridPos(pos), esJugador1(esJugador1), terminada(false), hitEnemigo(false) {
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
        if (!explotada) {
            // Animación de la bomba antes de explotar
            if (animationClock.getElapsedTime().asSeconds() >= 0.5f) {
                currentFrame = (currentFrame + 1) % numFrames;
                bombaSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
                animationClock.restart();
            }

            // Verificar si es tiempo de explotar
            if (timer.getElapsedTime().asSeconds() >= explosionTime) {
                explotada = true;
                enExpansion = true;
                currentFrame = 0; // Reiniciar frame para la animación de explosión
                animationClock.restart();
                explotar(mapa, jugador1, jugador2);
            }
        } 
        else if (enExpansion) {
            // Animación de la explosión
            if (animationClock.getElapsedTime().asSeconds() >= 0.1f) {
                currentFrame = (currentFrame + 1) % numFramesExplosion;
                
                // Actualizar frames de todos los sprites de explosión
                centerSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
                secondSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
                lastSprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
                
                animationClock.restart();

                // Terminar la explosión después de completar la animación
                if (currentFrame == numFramesExplosion - 1 && 
                    timer.getElapsedTime().asSeconds() >= explosionTime + 1.0f) {
                    terminada = true;
                    enExpansion = false;
                }
            }
        }
    }

    void explotar(Mapa& mapa, Personaje& jugador1, Personaje& jugador2) {
        // Centro de la explosión
        if (mapa.esDestructible(gridPos.x, gridPos.y)) {
            mapa.destruirBloque(gridPos.x, gridPos.y);
        }

        // Explotar en las cuatro direcciones
        for (int i = 1; i <= 2; ++i) {
            // Derecha
            if (gridPos.x + i < Mapa::ancho) {
                // Verificar si hay un bloque sólido que detenga la explosión
                if (mapa.grid[gridPos.y][gridPos.x + i] == Mapa::BLOQUE_SOLIDO || 
                    mapa.grid[gridPos.y][gridPos.x + i] == Mapa::BORDE_DER) {
                    break;
                }
                // Si es destructible, destruirlo y continuar la explosión
                if (mapa.esDestructible(gridPos.x + i, gridPos.y)) {
                    mapa.destruirBloque(gridPos.x + i, gridPos.y);
                }
                // Verificar colisión con jugadores
                checkPlayerCollision(gridPos.x + i, gridPos.y, jugador1, jugador2);
            }
        }

        // Izquierda
        for (int i = 1; i <= 2; ++i) {
            if (gridPos.x - i >= 0) {
                if (mapa.grid[gridPos.y][gridPos.x - i] == Mapa::BLOQUE_SOLIDO || 
                    mapa.grid[gridPos.y][gridPos.x - i] == Mapa::BORDE_IZQ) {
                    break;
                }
                if (mapa.esDestructible(gridPos.x - i, gridPos.y)) {
                    mapa.destruirBloque(gridPos.x - i, gridPos.y);
                }
                checkPlayerCollision(gridPos.x - i, gridPos.y, jugador1, jugador2);
            }
        }

        // Abajo
        for (int i = 1; i <= 2; ++i) {
            if (gridPos.y + i < Mapa::alto) {
                if (mapa.grid[gridPos.y + i][gridPos.x] == Mapa::BLOQUE_SOLIDO || 
                    mapa.grid[gridPos.y + i][gridPos.x] == Mapa::BORDE_ABAJO) {
                    break;
                }
                if (mapa.esDestructible(gridPos.x, gridPos.y + i)) {
                    mapa.destruirBloque(gridPos.x, gridPos.y + i);
                }
                checkPlayerCollision(gridPos.x, gridPos.y + i, jugador1, jugador2);
            }
        }

        // Arriba
        for (int i = 1; i <= 2; ++i) {
            if (gridPos.y - i >= 0) {
                if (mapa.grid[gridPos.y - i][gridPos.x] == Mapa::BLOQUE_SOLIDO || 
                    mapa.grid[gridPos.y - i][gridPos.x] == Mapa::BORDE_TOP) {
                    break;
                }
                if (mapa.esDestructible(gridPos.x, gridPos.y - i)) {
                    mapa.destruirBloque(gridPos.x, gridPos.y - i);
                }
                checkPlayerCollision(gridPos.x, gridPos.y - i, jugador1, jugador2);
            }
        }

        explotada = true;
        enExpansion = true;
    }

    void draw(sf::RenderWindow& window, Mapa& mapa) {
        if (!explotada) {
            bombaSprite.setPosition(gridPos.x * Mapa::tile, gridPos.y * Mapa::tile);
            window.draw(bombaSprite);
        } 
        else if (enExpansion) {
            // Centro de la explosión
            centerSprite.setPosition(gridPos.x * Mapa::tile, gridPos.y * Mapa::tile);
            window.draw(centerSprite);

            // Dibujar explosión en cruz
            for (int i = 1; i <= 2; ++i) {
                // Derecha
                if (validarPosicionExplosion(gridPos.x + i, gridPos.y, mapa)) {
                    sf::Sprite& sprite = (i == 1) ? secondSprite : lastSprite;
                    sprite.setRotation(0);
                    sprite.setPosition((gridPos.x + i) * Mapa::tile, gridPos.y * Mapa::tile);
                    window.draw(sprite);
                }

                // Izquierda
                if (validarPosicionExplosion(gridPos.x - i, gridPos.y, mapa)) {
                    sf::Sprite& sprite = (i == 1) ? secondSprite : lastSprite;
                    sprite.setRotation(180);
                    sprite.setPosition((gridPos.x - i) * Mapa::tile + Mapa::tile, gridPos.y * Mapa::tile + Mapa::tile);
                    window.draw(sprite);
                }

                // Abajo
                if (validarPosicionExplosion(gridPos.x, gridPos.y + i, mapa)) {
                    sf::Sprite& sprite = (i == 1) ? secondSprite : lastSprite;
                    sprite.setRotation(90);
                    sprite.setPosition(gridPos.x * Mapa::tile + Mapa::tile, (gridPos.y + i) * Mapa::tile);
                    window.draw(sprite);
                }

                // Arriba
                if (validarPosicionExplosion(gridPos.x, gridPos.y - i, mapa)) {
                    sf::Sprite& sprite = (i == 1) ? secondSprite : lastSprite;
                    sprite.setRotation(270);
                    sprite.setPosition(gridPos.x * Mapa::tile, (gridPos.y - i) * Mapa::tile + Mapa::tile);
                    window.draw(sprite);
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

    bool checkExplosion(Mapa& mapa, int x, int y) {
        if (x < 0 || x >= Mapa::ancho || y < 0 || y >= Mapa::alto) {
            return false;
        }

        int tile = mapa.grid[y][x];
        
        // Si es un bloque destructible
        if (tile == Mapa::BLOQUE_DESTRUCTIBLE || tile == Mapa::MURO_DESTRUCTIBLE) {
            mapa.destruirBloque(x, y);
            return false;  // Detener la explosión
        }
        
        // Si es un bloque sólido o borde
        if (tile == Mapa::BLOQUE_SOLIDO || 
            tile == Mapa::BORDE_IZQ || tile == Mapa::BORDE_TOP || 
            tile == Mapa::BORDE_DER_TOP || tile == Mapa::BORDE_DER || 
            tile == Mapa::BORDE_ABAJO || tile == Mapa::BORDE_ABAJO || 
            tile == Mapa::BORDE_IZQ_ABAJO) {
            return false;  // Detener la explosión
        }

        return true;  // Continuar la explosión
    }

    bool validarPosicionExplosion(int x, int y, const Mapa& mapa) {
        return x >= 0 && x < Mapa::ancho && y >= 0 && y < Mapa::alto && !mapa.esSolido(x, y);
    }

    void checkPlayerCollision(int x, int y, Personaje& jugador1, Personaje& jugador2) {
        // Verificar colisión con jugador 1
        if (sf::Vector2i(x, y) == sf::Vector2i(
            static_cast<int>(jugador1.getPosition().x / Mapa::tile),
            static_cast<int>(jugador1.getPosition().y / Mapa::tile))) {
            jugador1 = Personaje(sf::Vector2f(Mapa::tile, Mapa::tile));
            hitEnemigo = true;
        }
        // Verificar colisión con jugador 2
        if (sf::Vector2i(x, y) == sf::Vector2i(
            static_cast<int>(jugador2.getPosition().x / Mapa::tile),
            static_cast<int>(jugador2.getPosition().y / Mapa::tile))) {
            jugador2 = Personaje(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, 
                                            (Mapa::alto - 2) * Mapa::tile));
            hitEnemigo = true;
        }
    }
};
