#include "GameLogic.h"

// external functions - @krishanth's
int countLooseEnds(const Board &board);
int countComponents(const Board &board);
bool hasClosedLoop(const Board &board);

void applyMove(Board &board, const Move &move) {
  Tile &tile = board.grid[move.x][move.y];
  tile.rotation = (tile.rotation + move.rotation) % 360;
}

bool checkWin(const Board &board) {
  int looseEnds = countLooseEnds(board);
  int components = countComponents(board);
  bool loopExists = hasClosedLoop(board);

  if (looseEnds == 0 && components == 1 && !loopExists) {
    return true;
  }
  return false;
}
