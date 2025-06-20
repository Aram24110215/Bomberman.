#pragma once
#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include "AudioManager.h"


class Personaje {
public:
    bool estaVivo;
    
    Personaje(sf::Vector2f position, const std::string& imgPath = "assets/images/animacion.png") : estaVivo(true) {
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

    void Move(float offsetX, float offsetY, Mapa& mapa) {
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
        if (mapa.EsSolido(leftTile, topTile) || 
            mapa.EsSolido(rightTile, topTile) ||
            mapa.EsSolido(leftTile, bottomTile) || 
            mapa.EsSolido(rightTile, bottomTile)) {
            canMove = false;
        }

        // Si no hay colisión, mover el personaje y actualizar la animación
        if (canMove) {
            sprite.move(offsetX, offsetY);
            // Actualizar la animación según la dirección del movimiento
            if (offsetX > 0) {
                UpdateAnimation(0); // Derecha
            } else if (offsetX < 0) {
                UpdateAnimation(1); // Izquierda
            } else if (offsetY > 0) {
                UpdateAnimation(2); // Abajo
            } else if (offsetY < 0) {
                UpdateAnimation(3); // Arriba
            }
        }
    }

    void UpdateAnimation(int direction) {
        currentDirection = direction;
        if (animationClock.getElapsedTime().asSeconds() >= frameTime) {
            currentFrame = (currentFrame + 1) % numFrames;
            int frameX = currentFrame * frameWidth;
            int frameY = direction * frameHeight;
            sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
            animationClock.restart();
        }
    }

    void Draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::Vector2f GetPosition() const {
        return sprite.getPosition();
    }

    void RecogerPowerup(int tipo) {
        switch(tipo) {
            case Mapa::POWERUP_BOMBA:
                if (maxBombas < 8) maxBombas++;
                break;
            case Mapa::POWERUP_FUEGO:
                if (alcanceExplosion < 8) alcanceExplosion++;
                break;
            case Mapa::POWERUP_VELOCIDAD:
                if (velocidadBase < 120.0f) velocidadBase += 20.0f;
                break;
            case Mapa::POWERUP_PATEAR:
                puedePatear = true;
                break;
            case Mapa::POWERUP_ATRAVESAR:
                puedeAtravesar = true;
                break;
        }
        AudioManager::GetInstance().PlayPowerup();
    }

    void Morir() {
        estaVivo = false;
        sprite.setColor(sf::Color(128, 128, 128, 200)); // Efecto gris al morir
    }

    void ResetearPosicion(sf::Vector2f pos) {
        sprite.setPosition(pos);
        estaVivo = true;
        sprite.setColor(sf::Color::White);
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
    int numBombas = 1;
    int maxBombas = 1;
    int alcanceExplosion = 1;
    float velocidadBase = 60.0f;
    bool puedePatear = false;
    bool puedeAtravesar = false;
};