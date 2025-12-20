#include "Tile.h"

// Returns active ports based on tile type and rotation
vector<Direction> getActivePorts(const Tile &tile)
{
	vector<Direction> basePorts;

	// Define base ports (at 0° rotation)
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

	// Apply rotation
	vector<Direction> rotatedPorts;
	int rotSteps = tile.rotation / 90; // 0, 1, 2, 3

	for (Direction dir : basePorts)
	{
		int newDir = (dir + rotSteps) % 4;
		rotatedPorts.push_back(static_cast<Direction>(newDir));
	}

	return rotatedPorts;
}

// Get opposite direction
Direction opposite(Direction dir)
{
	return static_cast<Direction>((dir + 2) % 4);
}

// Rotate a direction by given degrees
Direction rotateDirection(Direction dir, int rotation)
{
	int steps = rotation / 90;
	return static_cast<Direction>((dir + steps) % 4);
}
