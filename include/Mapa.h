#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Mapa {
public:
    static const int ancho = 15, alto = 11;
    static const int tile = 16; // Tamaño de cada tile en píxeles
    int grid[alto][ancho] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
        {1,3,2,3,2,3,2,3,2,3,2,3,2,3,1},
        {1,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
        {1,3,2,3,2,3,2,3,2,3,2,3,2,3,1},
        {1,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
        {1,3,2,3,2,3,2,3,2,3,2,3,2,3,1},
        {1,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
        {1,3,2,3,2,3,2,3,2,3,2,3,2,3,1},
        {1,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    sf::Texture texturaTiles;
    sf::Sprite spriteTile;

    enum TipoMejora { VELOCIDAD, ALCANCE, BOMBA_EXTRA };

    struct Mejora {
        sf::Vector2f posicion;
        TipoMejora tipo;
        sf::Sprite sprite;

        Mejora(sf::Vector2f pos, TipoMejora t) : posicion(pos), tipo(t) {
            // Configurar el sprite de la mejora
            sprite.setPosition(pos);
            sprite.setTextureRect(sf::IntRect(static_cast<int>(t) * Mapa::tile, 0, Mapa::tile, Mapa::tile));
        }

        void draw(sf::RenderWindow& window) const { // Declarar como const
            window.draw(sprite);
        }

        sf::Vector2f getPosition() const { return posicion; }
        TipoMejora getTipo() const { return tipo; }
    };

    std::vector<Mejora> mejoras; // Añadir mejoras como miembro de la clase

    Mapa() {
        if (!texturaTiles.loadFromFile("assets/images/tileset.png")) {
            // Manejo de error
        }
        spriteTile.setTexture(texturaTiles);

        // Inicializar mejoras
        mejoras.emplace_back(sf::Vector2f(3 * tile, 3 * tile), VELOCIDAD);
        mejoras.emplace_back(sf::Vector2f(5 * tile, 5 * tile), ALCANCE);
        mejoras.emplace_back(sf::Vector2f(7 * tile, 7 * tile), BOMBA_EXTRA);

        for (auto& mejora : mejoras) {
            mejora.sprite.setTexture(texturaTiles); // Asignar la textura a cada mejora
        }
    }

    void draw(sf::RenderWindow& window) {
        for (int y = 0; y < alto; ++y) {
            for (int x = 0; x < ancho; ++x) {
                if (grid[y][x] == 1) {
                    spriteTile.setTextureRect(sf::IntRect(0, 0, tile, tile)); // Bloques sólidos
                } else if (grid[y][x] == 2) {
                    spriteTile.setTextureRect(sf::IntRect(tile, 0, tile, tile)); // Muros destructibles
                } else if (grid[y][x] == 3) {
                    spriteTile.setTextureRect(sf::IntRect(2 * tile, 0, tile, tile)); // Piso del mapa
                } else {
                    continue; // No dibujar celdas vacías
                }
                spriteTile.setPosition(x * tile, y * tile);
                window.draw(spriteTile);
            }
        }

        // Dibujar mejoras
        for (const auto& mejora : mejoras) {
            mejora.draw(window);
        }
    }

    bool esSolido(int x, int y) const {
        if (x < 0 || y < 0 || x >= ancho || y >= alto) return true; // Fuera del mapa
        return grid[y][x] == 1 || grid[y][x] == 2; // Bloques sólidos o destructibles
    }

    void destruirBloque(int x, int y) {
        if (x >= 0 && y >= 0 && x < ancho && y < alto && grid[y][x] == 2) {
            grid[y][x] = 3; // Cambiar el muro destructible a piso
        }
    }
};