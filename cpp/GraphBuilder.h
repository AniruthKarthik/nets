#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include "CpuStrategy.h"
#include "Tile.h"
#include <map>
#include <utility>
#include <vector>

using namespace std;

// Graph representation using adjacency list
struct Graph
{
	// Key: (row, col), Value: list of connected neighbors
	map<pair<int, int>, vector<pair<int, int>>> adjList;

	void addEdge(pair<int, int> a, pair<int, int> b)
	{
		adjList[a].push_back(b);
		adjList[b].push_back(a);
	}

	int nodeCount() const { return adjList.size(); }
};

// Build graph from board
Graph buildGraph(const Board &board);

// Helper to get neighbor position
pair<int, int> getNeighbor(int row, int col, Direction dir, int width,
                           int height, bool wraps);

#endif
