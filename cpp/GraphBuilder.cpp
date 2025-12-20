#include "GraphBuilder.h"
#include <iostream>

// Get neighbor position in given direction
pair<int, int> getNeighbor(int row, int col, Direction dir, int width,
                           int height, bool wraps)
{
	int newRow = row;
	int newCol = col;

	switch (dir)
	{
	case NORTH:
		newRow--;
		break;
	case SOUTH:
		newRow++;
		break;
	case EAST:
		newCol++;
		break;
	case WEST:
		newCol--;
		break;
	}

	// Handle wrapping
	if (wraps)
	{
		newRow = (newRow + height) % height;
		newCol = (newCol + width) % width;
	}
	else
	{
		// Check bounds
		if (newRow < 0 || newRow >= height || newCol < 0 || newCol >= width)
		{
			return {-1, -1}; // Invalid position
		}
	}

	return {newRow, newCol};
}

// Build graph from board
Graph buildGraph(const Board &board)
{
	Graph graph;

	// For each tile
	for (int row = 0; row < board.height; row++)
	{
		for (int col = 0; col < board.width; col++)
		{
			const Tile &tile = board.at(row, col);

			// Skip empty tiles
			if (tile.type == EMPTY)
				continue;

			// Add node to graph
			pair<int, int> currentPos = {row, col};
			if (graph.adjList.find(currentPos) == graph.adjList.end())
			{
				graph.adjList[currentPos] = vector<pair<int, int>>();
			}

			// Get active ports for this tile
			vector<Direction> ports = getActivePorts(tile);

			// Check each port
			for (Direction dir : ports)
			{
				// Get neighbor position
				pair<int, int> neighborPos = getNeighbor(
				    row, col, dir, board.width, board.height, board.wraps);

				// Check if neighbor exists
				if (neighborPos.first == -1)
					continue;

				const Tile &neighbor =
				    board.at(neighborPos.first, neighborPos.second);

				// Skip empty neighbors
				if (neighbor.type == EMPTY)
					continue;

				// Get neighbor's ports
				vector<Direction> neighborPorts = getActivePorts(neighbor);

				// Check if neighbor has opposite port
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

				// Add edge if connection exists
				if (hasConnection)
				{
					// Only add edge once (avoid duplicates)
					bool alreadyConnected = false;
					for (auto &neighbor : graph.adjList[currentPos])
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
