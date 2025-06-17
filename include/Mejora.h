#pragma once
#include <SFML/Graphics.hpp>
#include "Mapa.h"

class Mejora {
public:
    enum Tipo { VELOCIDAD, ALCANCE, BOMBA_EXTRA };

    Mejora(sf::Vector2f position, Tipo tipo) : tipo(tipo) {
        if (!textura.loadFromFile("assets/images/mejoras.png")) {
            // Manejo de error
        }
        sprite.setTexture(textura);
        sprite.setPosition(position);

        // Asignar la textura según el tipo de mejora
        switch (tipo) {
        case VELOCIDAD:
            sprite.setTextureRect(sf::IntRect(0, 0, Mapa::tile, Mapa::tile));
            break;
        case ALCANCE:
            sprite.setTextureRect(sf::IntRect(Mapa::tile, 0, Mapa::tile, Mapa::tile));
            break;
        case BOMBA_EXTRA:
            sprite.setTextureRect(sf::IntRect(2 * Mapa::tile, 0, Mapa::tile, Mapa::tile));
            break;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    Tipo getTipo() const {
        return tipo;
    }

    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

private:
    sf::Sprite sprite;
    sf::Texture textura;
    Tipo tipo;
};