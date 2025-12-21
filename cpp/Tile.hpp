#ifndef TILE_HPP
#define TILE_HPP

#include <utility>
#include <vector>

using namespace std;

enum Direction
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

enum TileType
{
	EMPTY,
	POWER,
	PC,
	STRAIGHT,
	CORNER,
	T_JUNCTION,
	CROSS
};

struct Tile
{
	TileType type;
	int rotation;
	bool locked;

	Tile() : type(EMPTY), rotation(0), locked(false) {}
	Tile(TileType t, int r, bool l = false) : type(t), rotation(r), locked(l) {}
};

struct Board
{
	int width;
	int height;
	bool wraps;
	vector<vector<Tile>> grid;
	pair<int, int> powerTile;

	Board(int w, int h, bool wrap = false)
	    : width(w), height(h), wraps(wrap), powerTile({-1, -1})
	{
		grid.resize(h, vector<Tile>(w));
	}

	Tile &at(int row, int col) { return grid[row][col]; }

	const Tile &at(int row, int col) const { return grid[row][col]; }
};

struct Move {
  int x;
  int y;
  int rotation;
};

// Implementations

inline vector<Direction> getActivePorts(const Tile &tile)
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

inline Direction opposite(Direction dir)
{
	return static_cast<Direction>((dir + 2) % 4);
}

inline Direction rotateDirection(Direction dir, int rotation)
{
	int steps = rotation / 90;
	return static_cast<Direction>((dir + steps) % 4);
}

inline pair<int, int> getNeighbor(int row, int col, Direction dir, int width,
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

#endif // TILE_HPP
