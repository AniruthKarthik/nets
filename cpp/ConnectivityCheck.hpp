#ifndef CONNECTIVITY_CHECK_HPP
#define CONNECTIVITY_CHECK_HPP

#include "GraphBuilder.hpp"
#include "Tile.hpp"
#include <set>
#include <iostream>

using namespace std;

// Implementations

// Time Complexity: O(V + E) where V is nodes, E is edges. Since max degree is 4, O(V).
// Space Complexity: O(V) for visited set and recursion stack.
inline void dfs(const Graph &graph, pair<int, int> node, vector<bool> &visited)
{
	visited[node.first * graph.width + node.second] = true;

	for (const auto &neighbor : graph.adjList[node.first * graph.width + node.second])
	{
		if (!visited[neighbor.first * graph.width + neighbor.second])
		{
			dfs(graph, neighbor, visited);
		}
	}
}

// Time Complexity: O(V)
// Space Complexity: O(V)
inline bool isFullyConnected(const Graph &graph, pair<int, int> powerSource)
{
	if (graph.nodeCount() == 0)
		return false;

	vector<bool> visited(graph.adjList.size(), false);
	dfs(graph, powerSource, visited);

    int visitedCount = 0;
    for (bool v : visited) if (v) visitedCount++;

	return visitedCount == graph.nodeCount();
}

// Time Complexity: O(V)
// Space Complexity: O(V)
inline int countComponents(const Graph &graph)
{
	vector<bool> visited(graph.adjList.size(), false);
	int components = 0;

	for (int i = 0; i < (int)graph.adjList.size(); ++i)
	{
		if (!visited[i] && graph.nodesExist[i])
		{
			components++;
            int r = i / graph.width;
            int c = i % graph.width;
			dfs(graph, {r, c}, visited);
		}
	}
	return components;
}

// Time Complexity: O(V log V) due to buildGraph using std::map
// Space Complexity: O(V)
inline int countComponents(const Board &board)
{
	Graph graph = buildGraph(board);
	return countComponents(graph);
}

// Time Complexity: O(V) - Iterates all tiles once
// Space Complexity: O(1)
inline int countLooseEnds(const Board &board)
{
	int looseEnds = 0;

	for (int row = 0; row < board.height; row++)
	{
		for (int col = 0; col < board.width; col++)
		{
			const Tile &tile = board.at(row, col);

			if (tile.type == EMPTY || tile.type == POWER)
				continue;

			uint8_t mask = getActivePortsMask(tile);

			for (int d = 0; d < 4; d++)
			{
                if (!(mask & (1 << d))) continue;
                Direction dir = static_cast<Direction>(d);
				pair<int, int> neighborPos = getNeighbor(
				    row, col, dir, board.width, board.height, board.wraps);

				bool hasMatchingPort = false;

				if (neighborPos.first != -1)
				{
					const Tile &neighbor =
					    board.at(neighborPos.first, neighborPos.second);

					if (neighbor.type != EMPTY)
					{
						uint8_t neighborMask = getActivePortsMask(neighbor);
						Direction oppositeDir = opposite(dir);
                        if (neighborMask & (1 << oppositeDir)) {
                            hasMatchingPort = true;
                        }
					}
				}

				if (!hasMatchingPort)
				{
					looseEnds++;
				}
			}
		}
	}

	return looseEnds;
}

// Time Complexity: O(V)
// Space Complexity: O(V) for recursion stack
inline bool dfsCycleDetection(const Graph &graph, pair<int, int> node,
                       pair<int, int> parent, vector<bool> &visited)
{
	visited[node.first * graph.width + node.second] = true;

	for (const auto &neighbor : graph.adjList[node.first * graph.width + node.second])
	{
		if (!visited[neighbor.first * graph.width + neighbor.second])
		{
			if (dfsCycleDetection(graph, neighbor, node, visited))
			{
				return true;
			}
		}
		else if (neighbor != parent)
		{
			return true;
		}
	}

	return false;
}

// Time Complexity: O(V)
// Space Complexity: O(V)
inline bool hasClosedLoop(const Graph &graph, pair<int, int> startNode)
{
	vector<bool> visited(graph.adjList.size(), false);
	return dfsCycleDetection(graph, startNode, {-1, -1}, visited);
}

// Time Complexity: O(V)
// Space Complexity: O(V)
inline bool hasClosedLoop(const Graph &graph)
{
	vector<bool> visited(graph.adjList.size(), false);

	for (int i = 0; i < (int)graph.adjList.size(); ++i)
	{
		if (!visited[i] && graph.nodesExist[i])
		{
            int r = i / graph.width;
            int c = i % graph.width;
			if (dfsCycleDetection(graph, {r, c}, {-1, -1}, visited))
			{
				return true;
			}
		}
	}
	return false;
}

// Time Complexity: O(V log V) due to buildGraph
// Space Complexity: O(V)
inline bool hasClosedLoop(const Board &board)
{
	Graph graph = buildGraph(board);
	return hasClosedLoop(graph);
}

// Time Complexity: O(V log V) dominated by buildGraph
// Space Complexity: O(V)
inline bool isSolved(const Board &board)
{
	if (board.powerTile.first == -1)
	{
		// std::cout << "Error: No power source found!" << std::endl;
		return false;
	}

	Graph graph = buildGraph(board);

	int looseEnds = countLooseEnds(board);
	if (looseEnds > 0)
	{
		// std::cout << "Loose ends detected: " << looseEnds << std::endl;
		return false;
	}

	if (!isFullyConnected(graph, board.powerTile))
	{
		// std::cout << "Network is not fully connected!" << std::endl;
		return false;
	}

	if (hasClosedLoop(graph, board.powerTile))
	{
		// std::cout << "Closed loop detected!" << std::endl;
		return false;
	}

	// std::cout << "✓ Puzzle solved!" << std::endl;
	return true;
}

#endif // CONNECTIVITY_CHECK_HPP
