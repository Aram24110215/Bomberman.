#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Mapa.h"
#include "Personaje.h"
#include "Bomba.h"
#include "AudioManager.h"

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    VICTORY
};

class Game {
private:
    static constexpr float VELOCIDAD_BASE = 60.0f;
    static constexpr int TIEMPO_PARTIDA_DEFAULT = 180;
    static constexpr int PUNTOS_VICTORIA = 1000;
    static constexpr int PUNTOS_KILL = 100;

    sf::RenderWindow& window;
    GameState gameState;
    bool juegoTerminado;
    int tiempoPartida;
    int nivel;
    int puntajeJ1;
    int puntajeJ2;

    Mapa mapa;
    Personaje jugador1;
    Personaje jugador2;
    std::vector<Bomba> bombas;
    bool bombaActiva1;
    bool bombaActiva2;

    sf::Font font;
    sf::Text tiempoText;
    sf::Text puntajeText;
    sf::Text nivelText;

    sf::Clock tiempoClock;
    sf::Clock gameClock;

public:
    Game(sf::RenderWindow& window);

    void initializeUI();
    void configureText(sf::Text& text, const std::string& str, float x, float y);
    void update();
    void updateGame();
    void updatePlayerMovement(Personaje& jugador, float velocidad,
                             sf::Keyboard::Key up, sf::Keyboard::Key down,
                             sf::Keyboard::Key left, sf::Keyboard::Key right);
    void updateUI();
    void checkGameOver();
    void siguienteNivel();
    void handleInput(sf::Event& event);
    void handlePlayingInput(sf::Event& event);
    void resetGame();
    void render();
    void renderText(sf::Text& text, const std::string& str, float x, float y, bool centered = false);
    void renderMenu();
    void renderGame();
    void renderGameOver();
    void renderVictory();
};