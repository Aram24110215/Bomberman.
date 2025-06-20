#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>

class Mapa {
public:
    static const int ancho = 15, alto = 11;
    static const int tile = 16;

    enum TileType {
        BORDE_IZQ = 0,
        BORDE_TOP = 1,
        BORDE_DER_TOP = 2,
        BLOQUE_SOLIDO = 3,
        MURO_DESTRUCTIBLE = 4,
        BLOQUE_DESTRUCTIBLE = 5,
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

    Mapa() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        cargarTexturas();
        generarMapa();
    }

    void Draw(sf::RenderWindow& window) {
        for (int y = 0; y < alto; ++y) {
            for (int x = 0; x < ancho; ++x) {
                renderTile(window, x, y);
            }
        }
    }

    bool EsSolido(int x, int y) const {
        if (!esPosicionValida(x, y)) return true;
        return grid[y][x] != PISO && !esPowerup(x, y);
    }

    void DestruirBloque(int x, int y) {
        if (!esPosicionValida(x, y)) return;
        
        if (grid[y][x] == BLOQUE_DESTRUCTIBLE || grid[y][x] == MURO_DESTRUCTIBLE) {
            // 30% de probabilidad de powerup
            if (rand() % 100 < 30) {
                // Elegir un powerup aleatorio
                grid[y][x] = POWERUP_BOMBA + (rand() % 5);
            } else {
                grid[y][x] = PISO;
            }
        }
    }

    bool esDestructible(int x, int y) const {
        if (!esPosicionValida(x, y)) return false;
        int tile = grid[y][x];
        return tile == BLOQUE_DESTRUCTIBLE || tile == MURO_DESTRUCTIBLE;
    }

    bool EsDestructible(int x, int y) const {
        return esDestructible(x, y);
    }

    bool esPowerup(int x, int y) const {
        if (!esPosicionValida(x, y)) return false;
        return grid[y][x] >= POWERUP_BOMBA && grid[y][x] <= POWERUP_ATRAVESAR;
    }

    void reiniciarNivel() {
        generarMapa();
    }

    // Hacer el grid público pero const para lectura segura
    int grid[alto][ancho];

    // Método para obtener el valor de un tile
    int GetTile(int x, int y) const {
        if (esPosicionValida(x, y)) {
            return grid[y][x];
        }
        return -1;
    }

    // Método para establecer un tile
    void SetTile(int x, int y, int value) {
        if (esPosicionValida(x, y)) {
            grid[y][x] = value;
        }
    }
    
private:
    std::mt19937 rng;
    sf::Texture texturaTiles;
    sf::Sprite spriteTile;

    void cargarTexturas() {
        if (!texturaTiles.loadFromFile("assets/images/tileset.png")) {
            throw std::runtime_error("Error loading tileset.png");
        }
        spriteTile.setTexture(texturaTiles);
    }

    bool esZonaSegura(int x, int y) const {
        // Modificar para que solo proteja el área inmediata alrededor del spawn
        return (x <= 1 && y <= 1) ||  // Esquina superior izquierda
               (x >= ancho-2 && y >= alto-2);  // Esquina inferior derecha
    }

    void generarMapa() {
        const int mapaBase[alto][ancho] = {
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,2},    
            {8,6,6,5,5,5,5,5,5,5,5,5,6,6,8},    
            {8,6,3,5,3,5,3,5,3,5,3,5,3,6,8},    
            {8,5,5,5,5,5,5,5,5,5,5,5,5,5,8},    
            {8,5,3,5,3,5,3,5,3,5,3,5,3,5,8},
            {8,5,5,5,5,5,5,5,5,5,5,5,5,5,8},
            {8,5,3,5,3,5,3,5,3,5,3,5,3,5,8},
            {8,5,5,5,5,5,5,5,5,5,5,5,5,5,8},
            {8,6,3,5,3,5,3,5,3,5,3,5,3,6,8},
            {8,6,6,5,5,5,5,5,5,5,5,5,6,6,8},
            {11,9,9,9,9,9,9,9,9,9,9,9,9,9,3}
        };

        // Copiar el mapa base sin modificaciones adicionales
        for (int y = 0; y < alto; ++y) {
            for (int x = 0; x < ancho; ++x) {
                grid[y][x] = mapaBase[y][x];
            }
        }
    }

    void renderTile(sf::RenderWindow& window, int x, int y) {
        int tileValue = grid[y][x];
        int tileX = (tileValue % 8) * tile;
        int tileY = (tileValue / 8) * tile;
        
        spriteTile.setTextureRect(sf::IntRect(tileX, tileY, tile, tile));
        spriteTile.setPosition(x * tile, y * tile);
        window.draw(spriteTile);
    }

    bool esPosicionValida(int x, int y) const {
        return x >= 0 && x < ancho && y >= 0 && y < alto;
    }
};