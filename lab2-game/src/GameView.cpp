#include "GameView.hpp"
#include <cmath>

GameView::GameView(const std::string& fontPath) {
    m_fontOk = m_font.loadFromFile(fontPath);

    bool ok1 = m_sndExplosionBuf.loadFromFile("assets/explosion.wav");
    bool ok2 = m_sndCoinBuf.loadFromFile("assets/coin.wav");

    if (ok1) { 
        m_sndExplosion.setBuffer(m_sndExplosionBuf); 
        m_sndExplosion.setVolume(60.f); 
    }

    if (ok2) { 
        m_sndCoin.setBuffer(m_sndCoinBuf); 
        m_sndCoin.setVolume(50.f); 
    }

    m_audioOk = ok1 && ok2;
}

void GameView::draw(sf::RenderWindow& window, GameModel& model) {
    const int ts = model.tileSize();

    window.clear(sf::Color(25, 25, 30));

    for (int y=0; y<model.map().height(); ++y) {
        for (int x=0; x<model.map().width(); ++x) {
            sf::RectangleShape floor(sf::Vector2f((float)ts, (float)ts));
            floor.setPosition((float)(x*ts), (float)(y*ts));
            floor.setFillColor(sf::Color(35, 35, 45));
            window.draw(floor);

            if (auto* b = model.map().blockAt(x,y)) 
                b->draw(window, ts, x, y);
        }
    }

    for (auto& o : model.objects()) o->draw(window, ts);
    for (auto& e : model.effects()) e->draw(window, ts);

    model.player().draw(window, ts);

    drawHud(window, model);
    drawOverlay(window, model);

    if (m_audioOk) {
        while (model.popExplosionSoundEvent()) m_sndExplosion.play();
        while (model.popCoinSoundEvent()) m_sndCoin.play();
    }

    window.display();
}

void GameView::drawHud(sf::RenderWindow& window, const GameModel& model) {
    if (!m_fontOk) return;

    sf::Text t;
    t.setFont(m_font);
    t.setCharacterSize(24);
    t.setFillColor(sf::Color(220,220,230));

    std::string diff = (model.difficulty() == Difficulty::Easy ? "Easy" :
                        model.difficulty() == Difficulty::Normal ? "Normal" : "Hard");

    const auto& s = model.stats();
    t.setString(
        "WASD move | Space bomb | R restart | M/Esc menu | 1/2/3 difficulty | " +
        diff + " | base=" + std::to_string(s.baseScore)
    );
    t.setPosition(30.f, 8.f);
    window.draw(t);
}

void GameView::drawOverlay(sf::RenderWindow& window, const GameModel& model) {
    if (!m_fontOk) return;
    if (model.state() == GameState::Playing) return;

    sf::RectangleShape shade(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    shade.setFillColor(sf::Color(0,0,0,170));
    window.draw(shade);

    sf::Text title;
    title.setFont(m_font);
    title.setCharacterSize(76);
    title.setFillColor(sf::Color(250, 240, 220));

    if (model.state() == GameState::Menu) title.setString("Select difficulty: 1 | 2 | 3");
    else if (model.state() == GameState::Win) title.setString("YOU WIN!");
    else title.setString("YOU LOSE!");

    auto tb = title.getLocalBounds();
    title.setPosition(window.getSize().x/2.f - tb.width/2.f, 80.f);
    window.draw(title);

    sf::Text info;
    info.setFont(m_font);
    info.setCharacterSize(48);
    info.setFillColor(sf::Color(230,230,240));

    if (model.state() == GameState::Menu) {
        info.setString(
            "1 = Easy (small, few enemies, x1.0)\n"
            "2 = Normal (medium, x1.5)\n"
            "3 = Hard (big, many enemies, x2.0)\n"
            "\nPress 1/2/3 to start"
        );
        info.setPosition(120.f, 220.f);
        window.draw(info);
        return;
    }

    const auto& s = model.stats();
    int shownSeconds = (int)std::round(s.timeSeconds);

    std::string text =
        "Final score: " + std::to_string(s.finalScore) + "\n\n" +
        "Bricks destroyed: " + std::to_string(s.bricksDestroyed) + " (+10)\n" +
        "Enemies killed:   " + std::to_string(s.enemiesKilled) + " (+50)\n" +
        "Coins picked:     " + std::to_string(s.coinsPicked) + " (+5)\n" +
        "Bonuses picked:   " + std::to_string(s.bonusesPicked) + " (+8)\n\n" +
        "Bombs used:       " + std::to_string(s.bombsUsed) + " (-3)\n" +
        "Time:             " + std::to_string(shownSeconds) + " sec (-2/sec)\n\n" +
        "Difficulty mult:  x" + std::to_string(s.difficultyMultiplier) + "\n\n" +
        "Press R to restart\n"
        "Press M/Esc for menu";

    info.setString(text);
    info.setPosition(120.f, 220.f);
    window.draw(info);
}
