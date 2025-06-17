#pragma once
#include <SFML/Graphics.hpp>
#include "Mapa.h"

class Personaje {
public:
    Personaje(sf::Vector2f position, const std::string& imgPath = "assets/images/animacion.png") {
        sf::Image image;
        if (!image.loadFromFile(imgPath)) {
            // Error handling
        }
        // Set the specific green color (#70EA7A) as transparent
        image.createMaskFromColor(sf::Color(112, 234, 122));
        
        // Load the modified image into the texture
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        
        // Ajustar la escala para que coincida exactamente con el tamaño del tile
        sprite.setScale(1.0f, 1.0f); // Ya que frameWidth y Mapa::tile son ambos 16
    }

    void move(float offsetX, float offsetY, Mapa& mapa) {
        sf::Vector2f next = sprite.getPosition() + sf::Vector2f(offsetX, offsetY);

        // Verificar colisiones en las cuatro esquinas del sprite
        float spriteWidth = frameWidth * sprite.getScale().x;
        float spriteHeight = frameHeight * sprite.getScale().y;

        // Calcular las posiciones de las esquinas en la cuadrícula
        int leftTile = static_cast<int>(next.x / Mapa::tile);
        int rightTile = static_cast<int>((next.x + spriteWidth - 1) / Mapa::tile);
        int topTile = static_cast<int>(next.y / Mapa::tile);
        int bottomTile = static_cast<int>((next.y + spriteHeight - 1) / Mapa::tile);

        // Verificar si alguna de las esquinas colisiona con un bloque sólido
        bool canMove = true;
        if (mapa.esSolido(leftTile, topTile) || 
            mapa.esSolido(rightTile, topTile) ||
            mapa.esSolido(leftTile, bottomTile) || 
            mapa.esSolido(rightTile, bottomTile)) {
            canMove = false;
        }

        // Si no hay colisión, mover el personaje y actualizar la animación
        if (canMove) {
            sprite.move(offsetX, offsetY);
            
            // Actualizar la animación según la dirección del movimiento
            if (offsetX > 0) {
                updateAnimation(0); // Derecha
            } else if (offsetX < 0) {
                updateAnimation(1); // Izquierda
            } else if (offsetY > 0) {
                updateAnimation(2); // Abajo
            } else if (offsetY < 0) {
                updateAnimation(3); // Arriba
            }
        }
    }

    void updateAnimation(int direction) {
        currentDirection = direction;
        
        // Actualizar el frame de la animación
        if (animationClock.getElapsedTime().asSeconds() >= frameTime) {
            currentFrame = (currentFrame + 1) % numFrames;
            // Calcular la posición del frame en la spritesheet
            int frameX = currentFrame * frameWidth;
            int frameY = direction * frameHeight; // Cada dirección ocupa una fila de 16 píxeles
            sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
            animationClock.restart();
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

private:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Clock animationClock;
    float frameTime = 0.1f; // Reducido para una animación más fluida
    int currentFrame = 0;
    int currentDirection = 0;
    int numFrames = 3; // Número de frames por dirección
    static const int frameWidth = 16;  // Tamaño correcto del frame
    static const int frameHeight = 16; // Tamaño correcto del frame
    int alcanceExplosiones = 1;
    int maxBombas = 1;
};