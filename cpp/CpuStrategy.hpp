#ifndef CPU_STRATEGY_HPP
#define CPU_STRATEGY_HPP

#include "Tile.hpp"
#include "ConnectivityCheck.hpp"
#include "GameLogic.hpp"
#include <climits>
#include <vector>

using namespace std;

// Implementations

inline vector<Move> generateMoves(const Board &board) {
  vector<Move> moves;

  for (int i = 0; i < board.height; i++) {
    for (int j = 0; j < board.width; j++) {
      if (board.at(i, j).type == EMPTY) continue;
      
      // Only generate moves for non-locked tiles.
      if (!board.at(i, j).locked) {
           moves.push_back({i, j, 90});
           moves.push_back({i, j, 180});
           moves.push_back({i, j, 270});
      }
    }
  }
  return moves;
}

inline int evaluateBoard(const Board &board) {
  int looseEnds = countLooseEnds(board);
  int components = countComponents(board);
  bool hasLoop = hasClosedLoop(board);

  // Simple heuristic
  int score = (looseEnds * 10) + (components * 5) + (hasLoop ? 1000 : 0);
  return score;
}

inline Move chooseBestMove(const Board &board) {
  vector<Move> moves = generateMoves(board);
  if (moves.empty()) return {0,0,0}; 

  int bestScore = INT_MAX;
  Move bestMove = moves[0];

  for (const Move &move : moves) {
    Board tempBoard = board;
    applyMove(tempBoard, move);

    int score = evaluateBoard(tempBoard);
    if (score < bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }
  return bestMove;
}

#endif // CPU_STRATEGY_HPP
