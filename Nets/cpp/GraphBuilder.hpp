#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

#include "Tile.hpp"
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

// Implementation

inline Graph buildGraph(const Board &board)
{
	Graph graph;

	for (int row = 0; row < board.height; row++)
	{
		for (int col = 0; col < board.width; col++)
		{
			const Tile &tile = board.at(row, col);

			if (tile.type == EMPTY)
				continue;

			pair<int, int> currentPos = {row, col};
			if (graph.adjList.find(currentPos) == graph.adjList.end())
			{
				graph.adjList[currentPos] = vector<pair<int, int>>();
			}

			vector<Direction> ports = getActivePorts(tile);

			for (Direction dir : ports)
			{
				pair<int, int> neighborPos = getNeighbor(
				    row, col, dir, board.width, board.height, board.wraps);

				if (neighborPos.first == -1)
					continue;

				const Tile &neighbor =
				    board.at(neighborPos.first, neighborPos.second);

				if (neighbor.type == EMPTY)
					continue;

				vector<Direction> neighborPorts = getActivePorts(neighbor);
				Direction oppositeDir = opposite(dir);

				bool hasConnection = false;
				for (Direction nDir : neighborPorts)
				{
					if (nDir == oppositeDir)
					{
						hasConnection = true;
						break;
					}
				}

				if (hasConnection)
				{
					bool alreadyConnected = false;
					for (const auto &neighbor : graph.adjList[currentPos])
					{
						if (neighbor == neighborPos)
						{
							alreadyConnected = true;
							break;
						}
					}

					if (!alreadyConnected)
					{
						graph.addEdge(currentPos, neighborPos);
					}
				}
			}
		}
	}

	return graph;
}

#endif // GRAPH_BUILDER_HPP
