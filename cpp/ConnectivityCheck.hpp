#ifndef CONNECTIVITY_CHECK_HPP
#define CONNECTIVITY_CHECK_HPP

#include "GraphBuilder.hpp"
#include "Tile.hpp"
#include "SolverUtils.hpp"
#include <algorithm>
#include <vector>

using namespace std;

// Time Complexity: $O(V)$ where $V$ is number of tiles. Max degree is 4, so $E \le 4V$.
// Space Complexity: $O(V)$ for visited vector and recursion stack.
inline void dfs(const Graph &graph, int node, vector<bool> &visited)
{
	visited[node] = true;
	for (const auto &neighbor : graph.adjList[node])
	{
        int neighborIdx = neighbor.first * graph.width + neighbor.second;
		if (!visited[neighborIdx])
		{
			dfs(graph, neighborIdx, visited);
		}
	}
}

// Time Complexity: $O(V)$
// Space Complexity: $O(V)$
inline int countComponents(const Graph &graph)
{
	int count = 0;
	vector<bool> visited(graph.nodesExist.size(), false);
	for (size_t i = 0; i < graph.nodesExist.size(); i++)
	{
		if (graph.nodesExist[i] && !visited[i])
		{
			dfs(graph, i, visited);
			count++;
		}
	}
	return count;
}

// Time Complexity: $O(V)$
// Space Complexity: $O(V)$
inline int countVisitedNodes(const Graph &graph)
{
	vector<bool> visited(graph.nodesExist.size(), false);
	for (size_t i = 0; i < graph.nodesExist.size(); i++)
	{
		if (graph.nodesExist[i])
		{
			dfs(graph, i, visited);
			break; // Start from first existing node
		}
	}
	int count = 0;
	for (bool v : visited)
		if (v)
			count++;
	return count;
}

// Time Complexity: $O(V)$ (Optimized to avoid map lookups)
// Space Complexity: $O(V)$
inline int countLooseEnds(const Board &board)
{
	int looseEnds = 0;
	for (int r = 0; r < board.height; r++)
	{
		for (int c = 0; c < board.width; c++)
		{
			const Tile &tile = board.at(r, c);
			if (tile.type == EMPTY)
				continue;

			uint8_t mask = getActivePortsMask(tile);
			for (int d = 0; d < 4; d++)
			{
				if (!(mask & (1 << d)))
					continue;

				Direction dir = static_cast<Direction>(d);
				pair<int, int> nPos = getNeighbor(r, c, dir, board.width, board.height, board.wraps);

				if (nPos.first == -1)
				{
					looseEnds++;
					continue;
				}

				const Tile &nTile = board.at(nPos.first, nPos.second);
				if (nTile.type == EMPTY)
				{
					looseEnds++;
					continue;
				}

				uint8_t nMask = getActivePortsMask(nTile);
				if (!(nMask & (1 << opposite(dir))))
				{
					looseEnds++;
				}
			}
		}
	}
	return looseEnds;
}

// Time Complexity: $O(V)$
// Space Complexity: $O(V)$ for recursion stack
inline bool hasCycleDFS(const Graph &graph, int u, int p, vector<bool> &visited)
{
	visited[u] = true;
	for (const auto &neighbor : graph.adjList[u])
	{
        int v = neighbor.first * graph.width + neighbor.second;
		if (!visited[v])
		{
			if (hasCycleDFS(graph, v, u, visited))
				return true;
		}
		else if (v != p)
		{
			return true;
		}
	}
	return false;
}

// Time Complexity: $O(V)$
// Space Complexity: $O(V)$
inline bool hasClosedLoop(const Graph &graph)
{
	vector<bool> visited(graph.nodesExist.size(), false);
	for (size_t i = 0; i < graph.nodesExist.size(); i++)
	{
		if (graph.nodesExist[i] && !visited[i])
		{
			if (hasCycleDFS(graph, i, -1, visited))
				return true;
		}
	}
	return false;
}

// Time Complexity: $O(V)$
// Space Complexity: $O(V)$
inline bool isConnected(const Board &board)
{
	Graph graph = buildGraph(board);
	if (graph.nodeCount() == 0)
		return true;
	return countVisitedNodes(graph) == graph.nodeCount();
}

// Time Complexity: $O(V)$
// Space Complexity: $O(V)$
inline bool isSolved(const Board &board)
{
	if (countLooseEnds(board) > 0)
		return false;

	Graph graph = buildGraph(board);
	if (countVisitedNodes(graph) != graph.nodeCount())
		return false;

	if (hasClosedLoop(graph))
		return false;

	return true;
}

#endif // CONNECTIVITY_CHECK_HPP
