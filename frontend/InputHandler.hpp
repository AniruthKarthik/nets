#pragma once

#include <SFML/Graphics.hpp>

// Forward declare
struct Board;

class InputHandler {
public:
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, Board& board, bool is_solved);
};