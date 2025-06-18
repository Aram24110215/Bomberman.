#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Mapa {
public:
    static const int ancho = 15, alto = 11;
    static const int tile = 16;

    enum TileType {
        BORDE_IZQ = 0,
        BORDE_TOP = 1,
        BORDE_DER_TOP = 2,
        BORDE_DER_ABAJO = 3,
        MURO_DESTRUCTIBLE = 4,
        PISO = 6,
        BORDE_IZQ_ABAJO = 12,
        BORDE_ABAJO = 10,
        BORDE_DER = 11,
        POWERUP_BOMBA = 13,
        POWERUP_FUEGO = 14,
        POWERUP_VELOCIDAD = 15,
        POWERUP_PATEAR = 16,
        POWERUP_ATRAVESAR = 17
    };

    int grid[alto][ancho] = {
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,2},  // Primera fila con bordes
        {8,6,6,6,6,6,6,6,6,6,6,6,6,6,8},  // Filas del medio
        {8,6,4,6,4,6,4,6,4,6,4,6,4,6,8},
        {8,6,6,6,6,6,6,6,6,6,6,6,6,6,8},
        {8,6,4,6,4,6,4,6,4,6,4,6,4,6,8},
        {8,6,6,6,6,6,6,6,6,6,6,6,6,6,8},
        {8,6,4,6,4,6,4,6,4,6,4,6,4,6,8},
        {8,6,6,6,6,6,6,6,6,6,6,6,6,6,8},
        {8,6,4,6,4,6,4,6,4,6,4,6,4,6,8},
        {8,6,6,6,6,6,6,6,6,6,6,6,6,6,8},
        {11,9,9,9,9,9,9,9,9,9,9,9,9,9,3}  // Última fila con bordes
    };

    Mapa() {
        if (!texturaTiles.loadFromFile("assets/images/tileset.png")) {
            // Manejo de error
        }
        spriteTile.setTexture(texturaTiles);
    }

    void draw(sf::RenderWindow& window) {
        for (int y = 0; y < alto; ++y) {
            for (int x = 0; x < ancho; ++x) {
                int tileValue = grid[y][x];
                int tileX = (tileValue % 8) * tile;
                int tileY = (tileValue / 8) * tile;
                
                spriteTile.setTextureRect(sf::IntRect(tileX, tileY, tile, tile));
                spriteTile.setPosition(x * tile, y * tile);
                window.draw(spriteTile);
            }
        }
    }

    bool esSolido(int x, int y) const {
        if (x < 0 || y < 0 || x >= ancho || y >= alto) return true;
        int valor = grid[y][x];
        return valor != PISO; // Todo excepto el piso es sólido
    }

    void destruirBloque(int x, int y) {
        if (x >= 0 && y >= 0 && x < ancho && y < alto && grid[y][x] == MURO_DESTRUCTIBLE) {
            grid[y][x] = PISO;
        }
    }

    void generarPowerup(int x, int y) {
        if (rand() % 100 < 30) {
            int tipo = rand() % 5;
            grid[y][x] = POWERUP_BOMBA + tipo;
        } else {
            grid[y][x] = PISO;
        }
    }

    bool esPowerup(int x, int y) const {
        return grid[y][x] >= POWERUP_BOMBA && grid[y][x] <= POWERUP_ATRAVESAR;
    }

private:
    sf::Texture texturaTiles;
    sf::Sprite spriteTile;
};