#include "Renderer.hpp"
#include "../src/cpp/Tile.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

Renderer::Renderer() : m_win_text(m_font, "", 80), m_stats_text(m_font, "", 20) {
    if (!m_font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
        std::cerr << "Error: Could not load font!" << std::endl;
    }
    
    m_win_text.setString("You Win!");
    m_win_text.setFillColor(sf::Color::Green);
    m_win_text.setStyle(sf::Text::Bold);

    m_stats_text.setFillColor(sf::Color::White);
}

void Renderer::render(sf::RenderWindow& window, const Board& board, const std::set<std::pair<int, int>>& lit_tiles, bool is_solved, int loose_ends) {
    window.clear(sf::Color(40, 40, 40));

    const float TILE_SIZE = 80.0f;
    const float PADDING = 10.0f;

    for (int r = 0; r < board.height; ++r) {
        for (int c = 0; c < board.width; ++c) {
            const Tile& tile = board.at(r, c);
            bool is_lit = lit_tiles.count({r, c});
            bool is_power = (board.powerTile.first == r && board.powerTile.second == c);
            drawTile(window, tile, is_lit, is_power, PADDING + c * TILE_SIZE, PADDING + r * TILE_SIZE, TILE_SIZE);
        }
    }
    
    m_stats_text.setString("Loose Ends: " + std::to_string(loose_ends) + "\nSolved: " + (is_solved ? "Yes" : "No"));
    m_stats_text.setPosition({PADDING, PADDING + board.height * TILE_SIZE + PADDING});
    window.draw(m_stats_text);

    if (is_solved) {
        sf::FloatRect text_rect = m_win_text.getLocalBounds();
        m_win_text.setOrigin({text_rect.position.x + text_rect.size.x / 2.0f, text_rect.position.y + text_rect.size.y / 2.0f});
        m_win_text.setPosition({window.getSize().x / 2.0f, window.getSize().y / 2.0f});
        window.draw(m_win_text);
    }

    window.display();
}

void Renderer::drawTile(sf::RenderWindow& window, const Tile& tile, bool is_lit, bool is_power, float x_pos, float y_pos, float size) {
    sf::RectangleShape shape({size, size});
    shape.setPosition({x_pos, y_pos});
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color(50, 50, 50));

    if (is_power) {
        shape.setFillColor(sf::Color(255, 255, 100, 100));
    } else if (tile.locked) {
        shape.setFillColor(sf::Color(120, 120, 150));
    } else {
        shape.setFillColor(sf::Color(180, 180, 180));
    }
    window.draw(shape);
    
    if (tile.type == EMPTY) return;

    sf::VertexArray lines(sf::PrimitiveType::Lines);
    sf::Vector2f center(x_pos + size / 2.0f, y_pos + size / 2.0f);
    sf::Color line_color = is_lit ? sf::Color::Cyan : sf::Color(20, 20, 20);

    for (const auto& port : getActivePorts(tile)) {
        lines.append({center, line_color});
        switch (port) {
            case NORTH: lines.append({{center.x, y_pos}, line_color}); break;
            case EAST:  lines.append({{x_pos + size, center.y}, line_color}); break;
            case SOUTH: lines.append({{center.x, y_pos + size}, line_color}); break;
            case WEST:  lines.append({{x_pos, center.y}, line_color}); break;
        }
    }
    window.draw(lines);
}