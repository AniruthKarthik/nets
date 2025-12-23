#include "InputHandler.hpp"
#include "../src/cpp/GameLogic.hpp" // For applyMove
#include <SFML/Graphics.hpp>
#include <iostream>

void InputHandler::handleEvent(const sf::Event& event, sf::RenderWindow& window, Board& board, bool is_solved) {
    
    if (event.is<sf::Event::Closed>()) {
        window.close();
    }

    if (is_solved) {
        return; // Don't process game input if already solved
    }

    // --- Player Input ---
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i pixel_pos(mb->position.x, mb->position.y);
        sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);
        
        const float TILE_SIZE = 80.0f;
        const float PADDING = 10.0f;
        
        int col = static_cast<int>((world_pos.x - PADDING) / TILE_SIZE);
        int row = static_cast<int>((world_pos.y - PADDING) / TILE_SIZE);

        // Debug output
        std::cout << "Mouse: (" << mb->position.x << ", " << mb->position.y << "), "
                    << "World: (" << world_pos.x << ", " << world_pos.y << ") -> Grid: ("
                    << row << ", " << col << ")\n";

        if (col >= 0 && col < board.width && row >= 0 && row < board.height && board.at(row, col).type != EMPTY) {
            
            if (mb->button == sf::Mouse::Button::Middle) {
                board.at(row, col).locked = !board.at(row, col).locked;
            } else if (!board.at(row, col).locked) {
                if (mb->button == sf::Mouse::Button::Left) { // Clockwise
                    applyMove(board, {row, col, 90});
                } else if (mb->button == sf::Mouse::Button::Right) { // Anti-clockwise
                    applyMove(board, {row, col, 270}); // 270 is -90 degrees
                }
            }
        }
    }
}