#include "Tile.h"

vector<Direction> getActivePorts(const Tile &tile)
{
	vector<Direction> basePorts;

	switch (tile.type)
	{
	case EMPTY:
		break;

	case POWER:
	case CROSS:
		basePorts = {NORTH, EAST, SOUTH, WEST};
		break;

	case PC:
		basePorts = {NORTH};
		break;

	case STRAIGHT:
		basePorts = {NORTH, SOUTH};
		break;

	case CORNER:
		basePorts = {NORTH, EAST};
		break;

	case T_JUNCTION:
		basePorts = {NORTH, EAST, WEST};
		break;
	}

	vector<Direction> rotatedPorts;
	int rotSteps = tile.rotation / 90;

	for (Direction dir : basePorts)
	{
		int newDir = (dir + rotSteps) % 4;
		rotatedPorts.push_back(static_cast<Direction>(newDir));
	}

	return rotatedPorts;
}

Direction opposite(Direction dir)
{
	return static_cast<Direction>((dir + 2) % 4);
}

Direction rotateDirection(Direction dir, int rotation)
{
	int steps = rotation / 90;
	return static_cast<Direction>((dir + steps) % 4);
}

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

	if (wraps)
	{
		newRow = (newRow + height) % height;
		newCol = (newCol + width) % width;
	}
	else
	{
		if (newRow < 0 || newRow >= height || newCol < 0 || newCol >= width)
		{
			return {-1, -1};
		}
	}

	return {newRow, newCol};
}
