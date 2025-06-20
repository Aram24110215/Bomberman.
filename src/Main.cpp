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
        InitializeUI();
        AudioManager::GetInstance().PlayMusica();
    } catch (const std::exception& e) {
        std::cerr << "Error initializing game: " << e.what() << std::endl;
        throw;
    }
}

void Game::InitializeUI() {
    if (!font.loadFromFile("assets/fonts/pixel.ttf")) {
        throw std::runtime_error("Error loading font");
    }
    ConfigureText(tiempoText, "Tiempo: " + std::to_string(tiempoPartida), 10, 10);
    ConfigureText(puntajeText, "P1: 0  P2: 0", 100, 10);
    ConfigureText(nivelText, "Nivel " + std::to_string(nivel), 200, 10);
}

void Game::ConfigureText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(16);
    text.setPosition(x, y);
    text.setFillColor(sf::Color::White);
}

void Game::Update() {
    if (gameState == PLAYING && !juegoTerminado) {
        UpdateGame();
        UpdateUI();
        CheckGameOver();
    }
}

void Game::UpdateGame() {
    auto it = bombas.begin();
    while (it != bombas.end()) {
        it->Update(mapa, jugador1, jugador2);

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
        static_cast<int>(jugador1.GetPosition().x / Mapa::tile),
        static_cast<int>(jugador1.GetPosition().y / Mapa::tile)
    );
    int tile1 = mapa.GetTile(pos1.x, pos1.y);
    if (tile1 >= Mapa::POWERUP_BOMBA && tile1 <= Mapa::POWERUP_ATRAVESAR) {
        jugador1.RecogerPowerup(tile1);
        mapa.SetTile(pos1.x, pos1.y, Mapa::PISO);
        AudioManager::GetInstance().PlayPowerup();
    }

    sf::Vector2i pos2(
        static_cast<int>(jugador2.GetPosition().x / Mapa::tile),
        static_cast<int>(jugador2.GetPosition().y / Mapa::tile)
    );
    int tile2 = mapa.GetTile(pos2.x, pos2.y);
    if (tile2 >= Mapa::POWERUP_BOMBA && tile2 <= Mapa::POWERUP_ATRAVESAR) {
        jugador2.RecogerPowerup(tile2);
        mapa.SetTile(pos2.x, pos2.y, Mapa::PISO);
        AudioManager::GetInstance().PlayPowerup();
    }

    float deltaTime = gameClock.restart().asSeconds();
    float velocidad = VELOCIDAD_BASE * deltaTime;

    UpdatePlayerMovement(jugador1, velocidad, sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D);
    UpdatePlayerMovement(jugador2, velocidad, sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right);
}

void Game::UpdatePlayerMovement(Personaje& jugador, float velocidad,
                                sf::Keyboard::Key up, sf::Keyboard::Key down,
                                sf::Keyboard::Key left, sf::Keyboard::Key right) {
    if (sf::Keyboard::isKeyPressed(up)) jugador.Move(0, -velocidad, mapa);
    if (sf::Keyboard::isKeyPressed(down)) jugador.Move(0, velocidad, mapa);
    if (sf::Keyboard::isKeyPressed(left)) jugador.Move(-velocidad, 0, mapa);
    if (sf::Keyboard::isKeyPressed(right)) jugador.Move(velocidad, 0, mapa);
}

void Game::UpdateUI() {
    int tiempoRestante = tiempoPartida - static_cast<int>(tiempoClock.getElapsedTime().asSeconds());
    tiempoText.setString("Tiempo: " + std::to_string(tiempoRestante));
    puntajeText.setString("P1: " + std::to_string(puntajeJ1) +
                          "  P2: " + std::to_string(puntajeJ2));
}

void Game::CheckGameOver() {
    int tiempoRestante = tiempoPartida - static_cast<int>(tiempoClock.getElapsedTime().asSeconds());

    if (tiempoRestante <= 0 || (!jugador1.estaVivo && !jugador2.estaVivo)) {
        juegoTerminado = true;
        gameState = GAME_OVER;
    }
    else if (jugador1.estaVivo && !jugador2.estaVivo) {
        puntajeJ1 += PUNTOS_VICTORIA;
        SiguienteNivel();
    }
    else if (!jugador1.estaVivo && jugador2.estaVivo) {
        puntajeJ2 += PUNTOS_VICTORIA;
        SiguienteNivel();
    }
}

void Game::SiguienteNivel() {
    nivel++;
    mapa = Mapa();
    tiempoClock.restart();
    nivelText.setString("Nivel " + std::to_string(nivel));
    jugador1.ResetearPosicion(sf::Vector2f(Mapa::tile, Mapa::tile));
    jugador2.ResetearPosicion(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile, (Mapa::alto - 2) * Mapa::tile));
}

void Game::HandleInput(sf::Event& event) {
    switch(gameState) {
        case MENU:
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter) {
                gameState = PLAYING;
                tiempoClock.restart();
            }
            break;

        case PLAYING:
            HandlePlayingInput(event);
            break;

        case GAME_OVER:
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::R) {
                ResetGame();
            }
            break;

        case VICTORY:
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter) {
                SiguienteNivel();
                gameState = PLAYING;
            }
            break;
    }
}

void Game::HandlePlayingInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space && !bombaActiva1) {
            sf::Vector2i gridPos(
                static_cast<int>((jugador1.GetPosition().x + Mapa::tile/2) / Mapa::tile),
                static_cast<int>((jugador1.GetPosition().y + Mapa::tile/2) / Mapa::tile)
            );
            bombas.emplace_back(gridPos, true);
            bombaActiva1 = true;
            AudioManager::GetInstance().PlayExplosion();
        }
        else if (event.key.code == sf::Keyboard::Return && !bombaActiva2) {
            sf::Vector2i gridPos(
                static_cast<int>((jugador2.GetPosition().x + Mapa::tile/2) / Mapa::tile),
                static_cast<int>((jugador2.GetPosition().y + Mapa::tile/2) / Mapa::tile)
            );
            bombas.emplace_back(gridPos, false);
            bombaActiva2 = true;
            AudioManager::GetInstance().PlayExplosion();
        }
    }
}

void Game::ResetGame() {
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
    jugador1.ResetearPosicion(sf::Vector2f(Mapa::tile, Mapa::tile));
    jugador2.ResetearPosicion(sf::Vector2f((Mapa::ancho - 2) * Mapa::tile,
                                           (Mapa::alto - 2) * Mapa::tile));
    tiempoClock.restart();
    AudioManager::GetInstance().PlayMusica();
}

void Game::Render() {
    window.clear(sf::Color::Black);

    switch(gameState) {
        case MENU:
            RenderMenu();
            break;
        case PLAYING:
            RenderGame();
            break;
        case GAME_OVER:
            RenderGameOver();
            break;
        case VICTORY:
            RenderVictory();
            break;
    }

    window.display();
}

void Game::RenderText(sf::Text& text, const std::string& str, float x, float y, bool centered) {
    text.setString(str);
    if (centered) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    }
    text.setPosition(x, y);
    window.draw(text);
}

void Game::RenderMenu() {
    sf::Text menuText;
    ConfigureText(menuText, "BOMBERMAN\nPresiona ENTER para jugar",
                  window.getSize().x/2.0f, window.getSize().y/2.0f);
    RenderText(menuText, menuText.getString(), menuText.getPosition().x,
               menuText.getPosition().y, true);
}

void Game::RenderGame() {
    mapa.Draw(window);
    for (auto& bomba : bombas) {
        bomba.Draw(window, mapa);
    }
    jugador1.Draw(window);
    jugador2.Draw(window);

    window.draw(tiempoText);
    window.draw(puntajeText);
    window.draw(nivelText);
}

void Game::RenderGameOver() {
    sf::Text gameOverText;
    std::string winner = (puntajeJ1 > puntajeJ2) ? "¡Jugador 1 Gana!" :
                         (puntajeJ2 > puntajeJ1) ? "¡Jugador 2 Gana!" :
                         "¡Empate!";
    ConfigureText(gameOverText, "GAME OVER\n" + winner + "\nPresiona R para reiniciar",
                  window.getSize().x/2, window.getSize().y/2);
    gameOverText.setOrigin(gameOverText.getLocalBounds().width/2,
                           gameOverText.getLocalBounds().height/2);
    window.draw(gameOverText);
}

void Game::RenderVictory() {
    sf::Text victoryText;
    ConfigureText(victoryText, "¡NIVEL COMPLETADO!\nPresiona ENTER para continuar",
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
            game.HandleInput(event);
        }

        game.Update();
        game.Render();
    }

    return 0;
}