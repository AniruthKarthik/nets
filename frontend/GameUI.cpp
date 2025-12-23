#include "GameUI.hpp"
#include "PuzzleGenerator.hpp"
#include "../src/cpp/ConnectivityCheck.hpp"
#include "../src/cpp/GraphBuilder.hpp"

// Constants
const int GRID_WIDTH = 8;
const int GRID_HEIGHT = 8;
const float TILE_SIZE = 80.0f;
const float PADDING = 10.0f;

GameUI::GameUI() 
    : m_board(GRID_WIDTH, GRID_HEIGHT), m_is_solved(false), m_loose_ends(0)
{
    unsigned int window_width = (unsigned int)(GRID_WIDTH * TILE_SIZE + 2 * PADDING);
    unsigned int window_height = (unsigned int)(GRID_HEIGHT * TILE_SIZE + 2 * PADDING + 60);
    
    m_window.create(sf::VideoMode({window_width, window_height}), "Net Game");
    m_window.setFramerateLimit(60);

    resetGame();
}

void GameUI::run() {
    while (m_window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void GameUI::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        m_input_handler.handleEvent(*event, m_window, m_board, m_is_solved);
        
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::N) {
                resetGame();
            }
        }
    }
}

void GameUI::update() {
     m_loose_ends = countLooseEnds(m_board);
     m_is_solved = isSolved(m_board);
}

void GameUI::render() {
    updateLitState();
    m_renderer.render(m_window, m_board, m_lit_tiles, m_is_solved, m_loose_ends);
}

void GameUI::resetGame() {
    PuzzleGenerator::generate(m_board);
    m_is_solved = false;
    update(); // Recalculate stats
}

void GameUI::updateLitState() {
    m_lit_tiles.clear();
    if (m_board.powerTile.first == -1) return;

    Graph graph = buildGraph(m_board);
    if (graph.adjList.count(m_board.powerTile)) {
        dfs(graph, m_board.powerTile, m_lit_tiles);
    }
}