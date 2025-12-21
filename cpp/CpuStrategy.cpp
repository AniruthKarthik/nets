#include "CpuStrategy.h"
#include "GameLogic.h"
#include "ConnectivityCheck.h"
#include <climits>

// external functions - @krishanth's
// Removed manual declarations as we included ConnectivityCheck.h

vector<Move> generateMoves(const Board &board) {
  vector<Move> moves;

  for (int i = 0; i < board.height; i++) {
    for (int j = 0; j < board.width; j++) {
      moves.push_back({i, j, 90});
      moves.push_back({i, j, 180});
      moves.push_back({i, j, 270});
    }
  }
  return moves;
}

int evaluateBoard(const Board &board) {
  int looseEnds = countLooseEnds(board);
  int components = countComponents(board);
  bool hasLoop = hasClosedLoop(board);

  int score = (looseEnds * 10) + (components * 5) + (hasLoop ? 1000 : 0);
  return score;
}

Move chooseBestMove(const Board &board) {
  vector<Move> moves = generateMoves(board);

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
