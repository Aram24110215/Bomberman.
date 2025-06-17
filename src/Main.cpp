#include <SFML/Graphics.hpp>
#include <vector>
#include "Mapa.h"
#include "Personaje.h"
#include "Bomba.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(272, 208), "Bomberman 2 Jugadores");
    Mapa mapa;
    
    // Inicializar jugadores
    Personaje jugador1(sf::Vector2f(Mapa::tile, Mapa::tile), "assets/images/animacion.png");
    Personaje jugador2(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile), "assets/images/animacion.png");

    std::vector<Bomba> bombas;
    bool bombaActiva1 = false;
    bool bombaActiva2 = false;

    sf::Clock gameClock; // Cambiado de clock a gameClock y declarado correctamente como sf::Clock

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Colocar bomba del jugador 1 (Espacio)
            if (event.type == sf::Event::KeyPressed && 
                event.key.code == sf::Keyboard::Space && 
                !bombaActiva1) {
                sf::Vector2i gridPos(
                    static_cast<int>(jugador1.getPosition().x / Mapa::tile),
                    static_cast<int>(jugador1.getPosition().y / Mapa::tile)
                );
                bombas.emplace_back(gridPos);
                bombaActiva1 = true;
            }

            // Colocar bomba del jugador 2 (Enter)
            if (event.type == sf::Event::KeyPressed && 
                event.key.code == sf::Keyboard::Enter && 
                !bombaActiva2) {
                sf::Vector2i gridPos(
                    static_cast<int>(jugador2.getPosition().x / Mapa::tile),
                    static_cast<int>(jugador2.getPosition().y / Mapa::tile)
                );
                bombas.emplace_back(gridPos);
                bombaActiva2 = true;
            }
        }

        // Actualizar bombas y verificar explosiones terminadas
        auto it = bombas.begin();
        while (it != bombas.end()) {
            it->update(mapa, jugador1, jugador2);
            
            if (it->terminada) {
                // Verificar qué jugador puede colocar una nueva bomba
                if (it->esJugador1) {
                    bombaActiva1 = false;
                } else {
                    bombaActiva2 = false;
                }
                it = bombas.erase(it);
            } else {
                ++it;
            }
        }

        // Calcular deltaTime usando el reloj declarado correctamente
        float deltaTime = gameClock.restart().asSeconds();
        float velocidad = 60.0f * deltaTime;

        // Mover jugador 1
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            jugador1.move(0, -velocidad, mapa); // Arriba
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            jugador1.move(0, velocidad, mapa); // Abajo
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            jugador1.move(-velocidad, 0, mapa); // Izquierda
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            jugador1.move(velocidad, 0, mapa); // Derecha
        }

        // Mover jugador 2
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            jugador2.move(0, -velocidad, mapa); // Arriba
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            jugador2.move(0, velocidad, mapa); // Abajo
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            jugador2.move(-velocidad, 0, mapa); // Izquierda
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            jugador2.move(velocidad, 0, mapa); // Derecha
        }

        // Dibujar todo
        window.clear(sf::Color::Black);
        mapa.draw(window);
        for (auto& bomba : bombas) {
            bomba.draw(window, mapa);
        }
        jugador1.draw(window);
        jugador2.draw(window);
        window.display();
    }

    return 0;
}