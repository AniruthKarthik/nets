#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "ConnectivityCheck.hpp"
#include "Tile.hpp"

// Implementations

inline void applyMove(Board &board, const Move &move) {
  Tile &tile = board.grid[move.x][move.y];
  tile.rotation = (tile.rotation + move.rotation) % 360;
}

inline bool checkWin(const Board &board) {
  int looseEnds = countLooseEnds(board);
  int components = countComponents(board);
  bool loopExists = hasClosedLoop(board);

  if (looseEnds == 0 && components == 1 && !loopExists) {
    return true;
  }
  return false;
}

#endif // GAME_LOGIC_HPP
