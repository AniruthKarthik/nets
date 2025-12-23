#ifndef CPU_STRATEGY_HPP
#define CPU_STRATEGY_HPP

#include "Tile.hpp"
#include "ConnectivityCheck.hpp"
#include "GameLogic.hpp"
#include <climits>
#include <vector>

using namespace std;

// Implementations

inline vector<Move> generateMoves(const Board &board, pair<int, int> lastMovedTile) {
  vector<Move> moves;

  for (int i = 0; i < board.height; i++) {
    for (int j = 0; j < board.width; j++) {
      if (i == lastMovedTile.first && j == lastMovedTile.second) {
          continue;
      }

      if (board.at(i, j).type == EMPTY) continue;
      
      // Only generate moves for non-locked tiles.
      if (!board.at(i, j).locked) {
           int currentRot = board.at(i, j).rotation;
           int nextRot = (currentRot + 90) % 360;
           moves.push_back({i, j, nextRot});
      }
    }
  }
  return moves;
}

// Helper function to define the importance of each tile type
inline int getTileTypePriority(TileType type) {
    switch (type) {
        case T_JUNCTION: return 4;
        case CORNER:     return 3;
        case STRAIGHT:   return 2;
        case PC:         return 1;
        default:         return 0;
    }
}

// Swaps two Move objects
inline void swapMoves(Move &a, Move &b) {
    Move temp = a;
    a = b;
    b = temp;
}

// Partition function for Quick Sort
inline int quickSort_partition(vector<Move> &moves, int low, int high, const Board &board) {
    Move pivot = moves[high];
    TileType pivotType = board.at(pivot.x, pivot.y).type;
    int pivotPriority = getTileTypePriority(pivotType);
    
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        TileType jType = board.at(moves[j].x, moves[j].y).type;
        // Sort in descending order of priority
        if (getTileTypePriority(jType) > pivotPriority) {
            i++;
            swapMoves(moves[i], moves[j]);
        }
    }
    swapMoves(moves[i + 1], moves[high]);
    return (i + 1);
}

// Recursive Quick Sort function
inline void quickSort_recursive(vector<Move> &moves, int low, int high, const Board &board) {
    if (low < high) {
        int pi = quickSort_partition(moves, low, high, board);
        quickSort_recursive(moves, low, pi - 1, board);
        quickSort_recursive(moves, pi + 1, high, board);
    }
}

// Main function to initiate Quick Sort
inline void sortMoves(vector<Move> &moves, const Board &board) {
    if (!moves.empty()) {
        quickSort_recursive(moves, 0, moves.size() - 1, board);
    }
}

inline int evaluateBoard(const Board &board) {
  Graph graph = buildGraph(board);
  int looseEnds = countLooseEnds(board);
  int components = countComponents(graph);
  bool hasLoop = hasClosedLoop(graph);

  // Simple heuristic
  int score = (looseEnds * 10) + (components * 5) + (hasLoop ? 1000 : 0);
  return score;
}

inline Move chooseBestMove(const Board &board, pair<int, int> lastMovedTile) {
  vector<Move> moves = generateMoves(board, lastMovedTile);
  if (moves.empty()) {
      moves = generateMoves(board, {-1, -1});
      if(moves.empty()) return {0,0,0}; 
  }

  sortMoves(moves, board); // Sort the moves before evaluation

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
