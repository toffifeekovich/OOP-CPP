#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameModel.hpp"

class GameView {
public:
    explicit GameView(const std::string& fontPath);
    void draw(sf::RenderWindow& window, GameModel& model);

private:
    sf::Font m_font;
    bool m_fontOk = false;

    sf::SoundBuffer m_sndExplosionBuf;
    sf::SoundBuffer m_sndCoinBuf;
    sf::Sound m_sndExplosion;
    sf::Sound m_sndCoin;
    bool m_audioOk = false;

    void drawHud(sf::RenderWindow& window, const GameModel& model);
    void drawOverlay(sf::RenderWindow& window, const GameModel& model);
};
