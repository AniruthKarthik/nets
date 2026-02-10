#ifndef CPU_STRATEGY_HPP
#define CPU_STRATEGY_HPP

#include "ConnectivityCheck.hpp"
#include "GameLogic.hpp"
#include "Tile.hpp"
#include <climits>
#include <vector>

using namespace std;

// Implementations

// Time Complexity: O(V) where V is number of tiles
// Space Complexity: O(V) to store moves
inline vector<Move> generateMoves_greedy(const Board &board,
                                  pair<int, int> lastMovedTile) {
  vector<Move> moves;

  for (int i = 0; i < board.height; i++) {
    for (int j = 0; j < board.width; j++) {

      if (board.at(i, j).type == EMPTY)
        continue;

      if (!board.at(i, j).locked) {
        int currentRot = board.at(i, j).rotation;
        int nextRot = (currentRot + 90) % 360;
        moves.push_back({i, j, nextRot});
      }
    }
  }
  return moves;
}

// Time Complexity: O(1) (checks 4 neighbors)
inline int calculateLocalFit_greedy(const Board &board, const Move &move) {
  Tile t = board.at(move.x, move.y);
  t.rotation = move.rotation;

  vector<Direction> myPorts = getActivePorts(t);
  bool myConns[4] = {false, false, false, false};
  for (auto d : myPorts)
    myConns[d] = true;

  int score = 0;

  for (int d = 0; d < 4; d++) {
    Direction dir = static_cast<Direction>(d);
    pair<int, int> nPos = getNeighbor(move.x, move.y, dir, board.width,
                                      board.height, board.wraps);

    bool nConn = false;
    if (nPos.first != -1) {
      const Tile &nTile = board.at(nPos.first, nPos.second);
      if (nTile.type != EMPTY) {
        vector<Direction> nPorts = getActivePorts(nTile);
        Direction opp = opposite(dir);
        for (auto nd : nPorts) {
          if (nd == opp) {
            nConn = true;
            break;
          }
        }
      }
    }

    if (myConns[d] == nConn) {
      score++;
    }
  }
  return score;
}

// Time Complexity: O(1)
// Space Complexity: O(1)
inline void swapMoves(Move &a, Move &b) {
  Move temp = a;
  a = b;
  b = temp;
}

// Partition function for Quick Sort
// Time Complexity: O(M) where M is range [low, high]
// Space Complexity: O(1)
inline int quickSort_partition_greedy(vector<Move> &moves, int low, int high,
                               const Board &board) {
  Move pivot = moves[high];
  int pivotPriority = calculateLocalFit_greedy(board, pivot);

  int i = (low - 1);

  for (int j = low; j <= high - 1; j++) {
    // Sort in descending order of priority (higher fit first)
    if (calculateLocalFit_greedy(board, moves[j]) > pivotPriority) {
      i++;
      swapMoves(moves[i], moves[j]);
    }
  }
  swapMoves(moves[i + 1], moves[high]);
  return (i + 1);
}

// Recursive Quick Sort function
// Time Complexity: Best/Avg O(M log M), Worst O(M^2) where M is number of moves
// Space Complexity: O(log M) stack space
inline void quickSort_recursive_greedy(vector<Move> &moves, int low, int high,
                                const Board &board) {
  if (low < high) {
    int pi = quickSort_partition_greedy(moves, low, high, board);
    quickSort_recursive_greedy(moves, low, pi - 1, board);
    quickSort_recursive_greedy(moves, pi + 1, high, board);
  }
}

// Time Complexity: O(M log M)
// Space Complexity: O(log M)
inline void sortMoves_greedy(vector<Move> &moves, const Board &board) {
  if (!moves.empty()) {
    quickSort_recursive_greedy(moves, 0, moves.size() - 1, board);
  }
}

// Time Complexity: O(V log V) due to buildGraph
// Space Complexity: O(V)
inline int evaluateBoard_greedy(const Board &board) {
  Graph graph = buildGraph(board);
  int looseEnds = countLooseEnds(board);
  int components = countComponents(graph);
  bool hasLoop = hasClosedLoop(graph);

  // Simple heuristic
  int score = (looseEnds * 10) + (components * 5) + (hasLoop ? 1000 : 0);
  return score;
}

// Time Complexity: O(M * V log V) where M is number of moves (approx V), so
// O(V^2 log V) Space Complexity: O(V)
inline Move chooseBestMove_greedy(const Board &board, pair<int, int> lastMovedTile) {
  vector<Move> moves = generateMoves_greedy(board, lastMovedTile);
  if (moves.empty()) {
    moves = generateMoves_greedy(board, {-1, -1});
    if (moves.empty())
      return {0, 0, 0};
  }

  sortMoves_greedy(moves, board); // Sort the moves by local fit

  int bestScore = INT_MAX;
  Move bestMove = moves[0];

  for (const Move &move : moves) {
    Board tempBoard = board;
    applyMove(tempBoard, move);

    int score = evaluateBoard_greedy(tempBoard);

    // Apply penalty to discourage undo loops, UNLESS it solves the puzzle.
    if (move.x == lastMovedTile.first && move.y == lastMovedTile.second) {
      score += 1;
    }

    if (score < bestScore) {
      bestScore = score;
      bestMove = move;
    }

    // Optimization: If found a winning (or very near winning) state, stop
    // searching. Score 5 = 0 loose ends + 1 component + 0 loops. Score 6 =
    // Solved + 1 penalty point.
    if (score <= 6) {
      return bestMove;
    }
  }
  return bestMove;
}

#endif // CPU_STRATEGY_HPP
