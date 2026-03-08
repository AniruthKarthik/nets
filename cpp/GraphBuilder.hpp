#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

#include "Tile.hpp"
#include <map>
#include <utility>
#include <vector>

using namespace std;

struct Graph
{
	vector<vector<pair<int, int>>> adjList;
    vector<bool> nodesExist;
    int width;
    int _nodeCount = 0;

	Graph(int w, int h) : width(w) {
        adjList.resize(w * h);
        nodesExist.assign(w * h, false);
    }

	void addEdge(pair<int, int> a, pair<int, int> b)
	{
		adjList[a.first * width + a.second].push_back(b);
		adjList[b.first * width + b.second].push_back(a);
	}

	int nodeCount() const {
        return _nodeCount;
    }
};

// Implementation

inline Graph buildGraph(const Board &board)
{
	Graph graph(board.width, board.height);

	for (int row = 0; row < board.height; row++)
	{
		for (int col = 0; col < board.width; col++)
		{
			const Tile &tile = board.at(row, col);
			if (tile.type == EMPTY) continue;
            
            graph.nodesExist[row * board.width + col] = true;
            graph._nodeCount++;

			uint8_t mask = getActivePortsMask(tile);

			for (int d = 0; d < 4; d++)
			{
                if (!(mask & (1 << d))) continue;
                Direction dir = static_cast<Direction>(d);
				pair<int, int> neighborPos = getNeighbor(
				    row, col, dir, board.width, board.height, board.wraps);

				if (neighborPos.first == -1)
					continue;

				const Tile &neighbor =
				    board.at(neighborPos.first, neighborPos.second);

				if (neighbor.type == EMPTY)
					continue;

				uint8_t neighborMask = getActivePortsMask(neighbor);
				Direction oppositeDir = opposite(dir);

				if (neighborMask & (1 << oppositeDir))
				{
					// Check if already added (only add in one direction of traversal or check)
                    // To avoid double edges in the undirected graph, we only add if (row, col) < neighborPos
                    if (make_pair(row, col) < neighborPos) {
					    graph.addEdge({row, col}, neighborPos);
                    }
				}
			}
		}
	}

	return graph;
}

#endif // GRAPH_BUILDER_HPP
