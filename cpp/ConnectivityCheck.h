#ifndef CONNECTIVITY_CHECK_H
#define CONNECTIVITY_CHECK_H

#include "GraphBuilder.h"
#include "Tile.h"
#include <set>

using namespace std;

// Check if graph is fully connected from power source
bool isFullyConnected(const Graph &graph, pair<int, int> powerSource);

// Count loose ends (ports without matching neighbors)
int countLooseEnds(const Board &board);

// Detect closed loops (cycles in graph)
bool hasClosedLoop(const Graph &graph, pair<int, int> startNode);

// Check if board is solved
bool isSolved(const Board &board);

// Helper: DFS for connectivity
void dfs(const Graph &graph, pair<int, int> node, set<pair<int, int>> &visited);

// Helper: DFS for cycle detection
bool dfsCycleDetection(const Graph &graph, pair<int, int> node,
                       pair<int, int> parent, set<pair<int, int>> &visited);

#endif
