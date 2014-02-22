#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "ActionMap.hpp"
#include "TileMap.hpp"
#include <string>

class Game {
private:
    sf::RenderWindow window;
    sf::Event event;
    ActionMap<std::string> actions;
    TileMap map;
public:
    Game();
    void render();
    void processEvents();
    void run();
};

#endif // GAME_HPP
