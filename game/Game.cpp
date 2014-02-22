#include "Game.hpp"
#include <SFML/System/Time.hpp>

const sf::Time timePerFrame = sf::seconds(1.f/60.f);

Game::Game(): window(sf::VideoMode(800, 600), "Compo 2", sf::Style::Close) {
//    if(!map.loadFromFile("level1.tmx")) {
//        throw "something broke";
//    }
    map.loadFromFile("level1.tmx");
}

void Game::render() {
    window.clear();
    window.draw(map);
    window.display();
}

void Game::processEvents() {
    while(window.pollEvent(event)) {
        switch(event.type) {
        case sf::Event::Closed:
            window.close();
            break;
        default:
            break;
        }
    }
}

void Game::run() {
    sf::Clock clock;
    sf::Time deltaTime = sf::Time::Zero;

    while(window.isOpen()) {
        sf::Time elapsedTime = clock.restart();
        deltaTime += elapsedTime;
        while(deltaTime > timePerFrame) {
            deltaTime -= timePerFrame;
            processEvents();
        }
        render();
    }
}
