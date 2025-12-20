#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include "Tile.h"
#include <map>
#include <utility>
#include <vector>

using namespace std;

struct Graph
{
	map<pair<int, int>, vector<pair<int, int>>> adjList;

	void addEdge(pair<int, int> a, pair<int, int> b)
	{
		adjList[a].push_back(b);
		adjList[b].push_back(a);
	}

	int nodeCount() const { return adjList.size(); }
};

Graph buildGraph(const Board &board);

#endif // GRAPH_BUILDER_H
