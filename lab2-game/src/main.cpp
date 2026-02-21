#include <SFML/Graphics.hpp>
#include "GameModel.hpp"
#include "GameView.hpp"
#include "GameController.hpp"

int main() {
    const int tileSize = 72;

    GameModel model(tileSize);
    GameView view("assets/font.ttf");
    GameController controller;

    sf::RenderWindow window(
        sf::VideoMode(model.map().width() * tileSize, model.map().height() * tileSize),
        "BomberMVC (SFML)",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        controller.handleInput(model);

        auto wantW = (unsigned)(model.map().width() * tileSize);
        auto wantH = (unsigned)(model.map().height() * tileSize);
        if (window.getSize().x != wantW || window.getSize().y != wantH) {
            window.create(sf::VideoMode(wantW, wantH), "BomberMVC (SFML)",
                          sf::Style::Titlebar | sf::Style::Close);
            window.setVerticalSyncEnabled(true);
        }

        model.update(dt);
        view.draw(window, model);
    }
    return 0;
}
