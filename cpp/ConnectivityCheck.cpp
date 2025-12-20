#include "ConnectivityCheck.h"
#include <iostream>

// DFS traversal for connectivity check
void dfs(const Graph &graph, pair<int, int> node, set<pair<int, int>> &visited)
{
	visited.insert(node);

	// Find neighbors
	auto it = graph.adjList.find(node);
	if (it == graph.adjList.end())
		return;

	for (const auto &neighbor : it->second)
	{
		if (visited.find(neighbor) == visited.end())
		{
			dfs(graph, neighbor, visited);
		}
	}
}

// Check if graph is fully connected
bool isFullyConnected(const Graph &graph, pair<int, int> powerSource)
{
	if (graph.nodeCount() == 0)
		return false;

	set<pair<int, int>> visited;
	dfs(graph, powerSource, visited);

	// All nodes should be visited
	return visited.size() == graph.nodeCount();
}

// Count loose ends
int countLooseEnds(const Board &board)
{
	int looseEnds = 0;

	for (int row = 0; row < board.height; row++)
	{
		for (int col = 0; col < board.width; col++)
		{
			const Tile &tile = board.at(row, col);

			if (tile.type == EMPTY)
				continue;

			// Get active ports
			vector<Direction> ports = getActivePorts(tile);

			// Check each port
			for (Direction dir : ports)
			{
				// Get neighbor
				pair<int, int> neighborPos = getNeighbor(
				    row, col, dir, board.width, board.height, board.wraps);

				bool hasMatchingPort = false;

				if (neighborPos.first != -1)
				{
					const Tile &neighbor =
					    board.at(neighborPos.first, neighborPos.second);

					if (neighbor.type != EMPTY)
					{
						// Check if neighbor has opposite port
						vector<Direction> neighborPorts =
						    getActivePorts(neighbor);
						Direction oppositeDir = opposite(dir);

						for (Direction nDir : neighborPorts)
						{
							if (nDir == oppositeDir)
							{
								hasMatchingPort = true;
								break;
							}
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

// DFS for cycle detection
bool dfsCycleDetection(const Graph &graph, pair<int, int> node,
                       pair<int, int> parent, set<pair<int, int>> &visited)
{
	visited.insert(node);

	auto it = graph.adjList.find(node);
	if (it == graph.adjList.end())
		return false;

	for (const auto &neighbor : it->second)
	{
		if (visited.find(neighbor) == visited.end())
		{
			if (dfsCycleDetection(graph, neighbor, node, visited))
			{
				return true;
			}
		}
		else if (neighbor != parent)
		{
			// Found a back edge (cycle)
			return true;
		}
	}

	return false;
}

// Check for closed loops
bool hasClosedLoop(const Graph &graph, pair<int, int> startNode)
{
	set<pair<int, int>> visited;
	return dfsCycleDetection(graph, startNode, {-1, -1}, visited);
}

// Main validation function
bool isSolved(const Board &board)
{
	// Check if power tile exists
	if (board.powerTile.first == -1)
	{
		cout << "Error: No power source found!" << endl;
		return false;
	}

	// Build graph
	Graph graph = buildGraph(board);

	// Check 1: No loose ends
	int looseEnds = countLooseEnds(board);
	if (looseEnds > 0)
	{
		cout << "Loose ends detected: " << looseEnds << endl;
		return false;
	}

	// Check 2: Fully connected
	if (!isFullyConnected(graph, board.powerTile))
	{
		cout << "Network is not fully connected!" << endl;
		return false;
	}

	// Check 3: No closed loops
	if (hasClosedLoop(graph, board.powerTile))
	{
		cout << "Closed loop detected!" << endl;
		return false;
	}

	cout << "✓ Puzzle solved!" << endl;
	return true;
}
