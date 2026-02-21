#pragma once
#include <SFML/Graphics.hpp>
#include "GameModel.hpp"

class GameController {
public:
    void handleInput(GameModel& model);

private:
    bool m_spaceWasDown = false;
    bool m_rWasDown = false;
    bool m_1WasDown = false;
    bool m_2WasDown = false;
    bool m_3WasDown = false;
    bool m_mWasDown = false;
    bool m_escWasDown = false;
};
