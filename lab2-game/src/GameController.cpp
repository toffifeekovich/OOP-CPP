#include "GameController.hpp"
#include <chrono>

static unsigned makeSeed() {
    return (unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void GameController::handleInput(GameModel& model) {

    bool mDown = sf::Keyboard::isKeyPressed(sf::Keyboard::M);
    if (mDown && !m_mWasDown) model.goToMenu();
    m_mWasDown = mDown;

    bool escDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
    if (escDown && !m_escWasDown) model.goToMenu();
    m_escWasDown = escDown;


    bool k1 = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
    bool k2 = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
    bool k3 = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);

    if (k1 && !m_1WasDown) { 
        model.setDifficulty(Difficulty::Easy); 
        model.newGame(makeSeed()); 
    }
    
    if (k2 && !m_2WasDown) { 
        model.setDifficulty(Difficulty::Normal); 
        model.newGame(makeSeed()); 
    }

    if (k3 && !m_3WasDown) { 
        model.setDifficulty(Difficulty::Hard); 
        model.newGame(makeSeed()); 
    }

    m_1WasDown = k1; m_2WasDown = k2; m_3WasDown = k3;


    bool rDown = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
    if (rDown && !m_rWasDown) model.newGame(makeSeed());
    m_rWasDown = rDown;

    if (model.state() != GameState::Playing) {
        m_spaceWasDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        return;
    }


    int dx = 0, dy = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dy = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dy = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dx = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dx = 1;

    model.player().requestMove(dx, dy);


    bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceDown && !m_spaceWasDown) model.player().requestBomb();
    m_spaceWasDown = spaceDown;
}
