#ifndef CPU_STRATEGY_H
#define CPU_STRATEGY_H

#include <vector>
using namespace std;

enum TileType { POWER, PC, STRAIGHT, CORNER, T_JUNCTION, CROSS };

struct Tile {
  TileType type;
  int rotation;
};

struct Board {
  int rows;
  int cols;
  vector<vector<Tile>> grid;
};

struct Move {
  int x;
  int y;
  int rotation;
};

vector<Move> generateMoves(const Board &board);
int evaluateBoard(const Board &board);
Move chooseBestMove(const Board &board);

#endif
