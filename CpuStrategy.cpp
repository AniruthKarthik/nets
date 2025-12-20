#include "CpuStrategy.h"
#include "GameLogic.h"
#include <climits>

// external functions - @krishanth's
int countLooseEnds(const Board &board);
int countComponents(const Board &board);

vector<Move> generateMoves(const Board &board) {
  vector<Move> moves;

  for (int i = 0; i < board.rows; i++) {
    for (int j = 0; j < board.cols; j++) {
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

  int score = (looseEnds * 10) + (components * 5);
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
