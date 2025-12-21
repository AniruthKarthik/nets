#ifndef CONNECTIVITY_CHECK_H
#define CONNECTIVITY_CHECK_H

#include "GraphBuilder.h"
#include "Tile.h"
#include <set>

using namespace std;

bool isFullyConnected(const Graph &graph, pair<int, int> powerSource);
int countLooseEnds(const Board &board);
int countComponents(const Board &board);
bool hasClosedLoop(const Graph &graph, pair<int, int> startNode);
bool hasClosedLoop(const Board &board);
bool isSolved(const Board &board);

void dfs(const Graph &graph, pair<int, int> node, set<pair<int, int>> &visited);
bool dfsCycleDetection(const Graph &graph, pair<int, int> node,
                       pair<int, int> parent, set<pair<int, int>> &visited);

#endif // CONNECTIVITY_CHECK_H
