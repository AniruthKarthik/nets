#pragma once

#include "InputHandler.hpp"
#include "Renderer.hpp"
#include "../src/cpp/Tile.hpp" // Needs full definition for Board member
#include <SFML/Graphics.hpp>
#include <set>
#include <utility>

class GameUI {
public:
    GameUI();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void resetGame();
    void updateLitState();

    sf::RenderWindow m_window;
    Board m_board;
    InputHandler m_input_handler;
    Renderer m_renderer;

    std::set<std::pair<int, int>> m_lit_tiles;
    bool m_is_solved;
    int m_loose_ends;
};
