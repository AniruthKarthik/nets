#pragma once

#include <SFML/Graphics.hpp>
#include <set>
#include <utility>

// Forward declare
struct Board;
struct Tile;

class Renderer {
public:
    Renderer(); // Constructor to load resources
    void render(sf::RenderWindow& window, const Board& board, const std::set<std::pair<int, int>>& lit_tiles, bool is_solved, int loose_ends);

private:
    void drawTile(sf::RenderWindow& window, const Tile& tile, bool is_lit, bool is_power, float x_pos, float y_pos, float size);
    
    sf::Font m_font;
    sf::Text m_win_text;
    sf::Text m_stats_text;
};
