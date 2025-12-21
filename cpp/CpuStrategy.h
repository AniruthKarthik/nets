#ifndef CPU_STRATEGY_H
#define CPU_STRATEGY_H

#include "Tile.h"
#include <vector>
using namespace std;

struct Move {
  int x;
  int y;
  int rotation;
};

vector<Move> generateMoves(const Board &board);
int evaluateBoard(const Board &board);
Move chooseBestMove(const Board &board);

#endif
