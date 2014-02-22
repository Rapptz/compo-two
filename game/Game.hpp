#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

class Game {
private:
    sf::RenderWindow window;
    sf::Event event;
public:
    Game();
    void render();
    void processEvents();
    void run();
};

#endif // GAME_HPP
