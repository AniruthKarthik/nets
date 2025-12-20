#include "ConnectivityCheck.h"
#include <iostream>

void dfs(const Graph &graph, pair<int, int> node, set<pair<int, int>> &visited)
{
	visited.insert(node);

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

bool isFullyConnected(const Graph &graph, pair<int, int> powerSource)
{
	if (graph.nodeCount() == 0)
		return false;

	set<pair<int, int>> visited;
	dfs(graph, powerSource, visited);

	return visited.size() == graph.nodeCount();
}

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

			vector<Direction> ports = getActivePorts(tile);

			for (Direction dir : ports)
			{
				pair<int, int> neighborPos = getNeighbor(
				    row, col, dir, board.width, board.height, board.wraps);

				bool hasMatchingPort = false;

				if (neighborPos.first != -1)
				{
					const Tile &neighbor =
					    board.at(neighborPos.first, neighborPos.second);

					if (neighbor.type != EMPTY)
					{
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
			return true;
		}
	}

	return false;
}

bool hasClosedLoop(const Graph &graph, pair<int, int> startNode)
{
	set<pair<int, int>> visited;
	return dfsCycleDetection(graph, startNode, {-1, -1}, visited);
}

bool isSolved(const Board &board)
{
	if (board.powerTile.first == -1)
	{
		std::cout << "Error: No power source found!" << std::endl;
		return false;
	}

	Graph graph = buildGraph(board);

	int looseEnds = countLooseEnds(board);
	if (looseEnds > 0)
	{
		std::cout << "Loose ends detected: " << looseEnds << std::endl;
		return false;
	}

	if (!isFullyConnected(graph, board.powerTile))
	{
		std::cout << "Network is not fully connected!" << std::endl;
		return false;
	}

	if (hasClosedLoop(graph, board.powerTile))
	{
		std::cout << "Closed loop detected!" << std::endl;
		return false;
	}

	std::cout << "✓ Puzzle solved!" << std::endl;
	return true;
}
