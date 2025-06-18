#include <SFML/Graphics.hpp>
#include <vector>
#include "Mapa.h"
#include "Personaje.h"
#include "Bomba.h"
#include "AudioManager.h"
#include "Game.h"
#include <iostream>

// --- INICIO: Implementación de métodos de Game (antes en Game.cpp) ---

Game::Game(sf::RenderWindow& window)
    : window(window),
      gameState(MENU),
      juegoTerminado(false),
      tiempoPartida(TIEMPO_PARTIDA_DEFAULT),
      nivel(1),
      puntajeJ1(0),
      puntajeJ2(0),
      jugador1(sf::Vector2f(Mapa::tile, Mapa::tile), "assets/images/animacion.png"),
      jugador2(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile), "assets/images/animacion.png"),
      bombaActiva1(false),
      bombaActiva2(false)
{
    try {
        initializeUI();
        AudioManager::getInstance().playMusica();
    } catch (const std::exception& e) {
        std::cerr << "Error initializing game: " << e.what() << std::endl;
        throw;
    }
}

void Game::initializeUI() {
    if (!font.loadFromFile("assets/fonts/pixel.ttf")) {
        throw std::runtime_error("Error loading font");
    }
    configureText(tiempoText, "Tiempo: " + std::to_string(tiempoPartida), 10, 10);
    configureText(puntajeText, "P1: 0  P2: 0", 100, 10);
    configureText(nivelText, "Nivel " + std::to_string(nivel), 200, 10);
}

void Game::configureText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(16);
    text.setPosition(x, y);
    text.setFillColor(sf::Color::White);
}

void Game::update() {
    if (gameState == PLAYING && !juegoTerminado) {
        updateGame();
        updateUI();
        checkGameOver();
    }
}

void Game::updateGame() {
    auto it = bombas.begin();
    while (it != bombas.end()) {
        it->update(mapa, jugador1, jugador2);

        if (it->terminada) {
            if (it->esJugador1) {
                bombaActiva1 = false;
                if (it->hitEnemigo) puntajeJ1 += PUNTOS_KILL;
            } else {
                bombaActiva2 = false;
                if (it->hitEnemigo) puntajeJ2 += PUNTOS_KILL;
            }
            it = bombas.erase(it);
        } else {
            ++it;
        }
    }

    sf::Vector2i pos1(
        static_cast<int>(jugador1.getPosition().x / Mapa::tile),
        static_cast<int>(jugador1.getPosition().y / Mapa::tile)
    );
    int tile1 = mapa.getTile(pos1.x, pos1.y);
    if (tile1 >= Mapa::POWERUP_BOMBA && tile1 <= Mapa::POWERUP_ATRAVESAR) {
        jugador1.recogerPowerup(tile1);
        mapa.setTile(pos1.x, pos1.y, Mapa::PISO);
        AudioManager::getInstance().playPowerup();
    }

    sf::Vector2i pos2(
        static_cast<int>(jugador2.getPosition().x / Mapa::tile),
        static_cast<int>(jugador2.getPosition().y / Mapa::tile)
    );
    int tile2 = mapa.getTile(pos2.x, pos2.y);
    if (tile2 >= Mapa::POWERUP_BOMBA && tile2 <= Mapa::POWERUP_ATRAVESAR) {
        jugador2.recogerPowerup(tile2);
        mapa.setTile(pos2.x, pos2.y, Mapa::PISO);
        AudioManager::getInstance().playPowerup();
    }

    float deltaTime = gameClock.restart().asSeconds();
    float velocidad = VELOCIDAD_BASE * deltaTime;

    updatePlayerMovement(jugador1, velocidad, sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D);
    updatePlayerMovement(jugador2, velocidad, sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right);
}

void Game::updatePlayerMovement(Personaje& jugador, float velocidad,
                                sf::Keyboard::Key up, sf::Keyboard::Key down,
                                sf::Keyboard::Key left, sf::Keyboard::Key right) {
    if (sf::Keyboard::isKeyPressed(up)) jugador.move(0, -velocidad, mapa);
    if (sf::Keyboard::isKeyPressed(down)) jugador.move(0, velocidad, mapa);
    if (sf::Keyboard::isKeyPressed(left)) jugador.move(-velocidad, 0, mapa);
    if (sf::Keyboard::isKeyPressed(right)) jugador.move(velocidad, 0, mapa);
}

void Game::updateUI() {
    int tiempoRestante = tiempoPartida - static_cast<int>(tiempoClock.getElapsedTime().asSeconds());
    tiempoText.setString("Tiempo: " + std::to_string(tiempoRestante));
    puntajeText.setString("P1: " + std::to_string(puntajeJ1) +
                          "  P2: " + std::to_string(puntajeJ2));
}

void Game::checkGameOver() {
    int tiempoRestante = tiempoPartida - static_cast<int>(tiempoClock.getElapsedTime().asSeconds());

    if (tiempoRestante <= 0 || (!jugador1.estaVivo && !jugador2.estaVivo)) {
        juegoTerminado = true;
        gameState = GAME_OVER;
        AudioManager::getInstance().stopMusica();
    }
    else if (jugador1.estaVivo && !jugador2.estaVivo) {
        puntajeJ1 += PUNTOS_VICTORIA;
        siguienteNivel();
    }
    else if (!jugador1.estaVivo && jugador2.estaVivo) {
        puntajeJ2 += PUNTOS_VICTORIA;
        siguienteNivel();
    }
}

void Game::siguienteNivel() {
    nivel++;
    mapa = Mapa();
    tiempoClock.restart();
    nivelText.setString("Nivel " + std::to_string(nivel));
    jugador1.resetearPosicion(sf::Vector2f(Mapa::tile, Mapa::tile));
    jugador2.resetearPosicion(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile));
}

void Game::handleInput(sf::Event& event) {
    switch(gameState) {
        case MENU:
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter) {
                gameState = PLAYING;
                tiempoClock.restart();
            }
            break;

        case PLAYING:
            handlePlayingInput(event);
            break;

        case GAME_OVER:
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::R) {
                resetGame();
            }
            break;

        case VICTORY:
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter) {
                siguienteNivel();
                gameState = PLAYING;
            }
            break;
    }
}

void Game::handlePlayingInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space && !bombaActiva1) {
            sf::Vector2i gridPos(
                static_cast<int>((jugador1.getPosition().x + Mapa::tile/2) / Mapa::tile),
                static_cast<int>((jugador1.getPosition().y + Mapa::tile/2) / Mapa::tile)
            );
            bombas.emplace_back(gridPos, true);
            bombaActiva1 = true;
            AudioManager::getInstance().playExplosion();
        }
        else if (event.key.code == sf::Keyboard::Return && !bombaActiva2) {
            sf::Vector2i gridPos(
                static_cast<int>((jugador2.getPosition().x + Mapa::tile/2) / Mapa::tile),
                static_cast<int>((jugador2.getPosition().y + Mapa::tile/2) / Mapa::tile)
            );
            bombas.emplace_back(gridPos, false);
            bombaActiva2 = true;
            AudioManager::getInstance().playExplosion();
        }
    }
}

void Game::resetGame() {
    gameState = MENU;
    juegoTerminado = false;
    nivel = 1;
    puntajeJ1 = 0;
    puntajeJ2 = 0;
    tiempoPartida = TIEMPO_PARTIDA_DEFAULT;
    bombaActiva1 = false;
    bombaActiva2 = false;
    bombas.clear();
    mapa = Mapa();
    jugador1.resetearPosicion(sf::Vector2f(Mapa::tile, Mapa::tile));
    jugador2.resetearPosicion(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile,
                                           (Mapa::alto - 2) * Mapa::tile));
    tiempoClock.restart();
    AudioManager::getInstance().playMusica();
}

void Game::render() {
    window.clear(sf::Color::Black);

    switch(gameState) {
        case MENU:
            renderMenu();
            break;
        case PLAYING:
            renderGame();
            break;
        case GAME_OVER:
            renderGameOver();
            break;
        case VICTORY:
            renderVictory();
            break;
    }

    window.display();
}

void Game::renderText(sf::Text& text, const std::string& str, float x, float y, bool centered) {
    text.setString(str);
    if (centered) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    }
    text.setPosition(x, y);
    window.draw(text);
}

void Game::renderMenu() {
    sf::Text menuText;
    configureText(menuText, "BOMBERMAN\nPresiona ENTER para jugar",
                  window.getSize().x/2.0f, window.getSize().y/2.0f);
    renderText(menuText, menuText.getString(), menuText.getPosition().x,
               menuText.getPosition().y, true);
}

void Game::renderGame() {
    mapa.draw(window);
    for (auto& bomba : bombas) {
        bomba.draw(window, mapa);
    }
    jugador1.draw(window);
    jugador2.draw(window);

    window.draw(tiempoText);
    window.draw(puntajeText);
    window.draw(nivelText);
}

void Game::renderGameOver() {
    sf::Text gameOverText;
    std::string winner = (puntajeJ1 > puntajeJ2) ? "¡Jugador 1 Gana!" :
                         (puntajeJ2 > puntajeJ1) ? "¡Jugador 2 Gana!" :
                         "¡Empate!";
    configureText(gameOverText, "GAME OVER\n" + winner + "\nPresiona R para reiniciar",
                  window.getSize().x/2, window.getSize().y/2);
    gameOverText.setOrigin(gameOverText.getLocalBounds().width/2,
                           gameOverText.getLocalBounds().height/2);
    window.draw(gameOverText);
}

void Game::renderVictory() {
    sf::Text victoryText;
    configureText(victoryText, "¡NIVEL COMPLETADO!\nPresiona ENTER para continuar",
                  window.getSize().x/2, window.getSize().y/2);
    victoryText.setOrigin(victoryText.getLocalBounds().width/2,
                          victoryText.getLocalBounds().height/2);
    window.draw(victoryText);
}

// --- FIN: Implementación de métodos de Game ---

int main() {
    sf::RenderWindow window(sf::VideoMode(272, 208), "Bomberman 2 Jugadores");
    window.setFramerateLimit(60);

    Game game(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            game.handleInput(event);
        }

        game.update();
        game.render();
    }

    return 0;
}