#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "Tile.hpp"

// Implementations

inline void applyMove(Board &board, const Move &move) {
  Tile &tile = board.grid[move.x][move.y];
  tile.rotation = move.rotation;
}

#endif // GAME_LOGIC_HPP
