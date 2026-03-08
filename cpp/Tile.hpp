#ifndef TILE_HPP
#define TILE_HPP

#include <utility>
#include <vector>
#include <cstdint>

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
    vector<bool> customConnections;

	Tile() : type(EMPTY), rotation(0), locked(false) {}
	Tile(TileType t, int r, bool l = false) : type(t), rotation(r), locked(l) {}
};

struct Board
{
	int width;
	int height;
	bool wraps;
	vector<Tile> grid;
	pair<int, int> powerTile;

	Board(int w, int h, bool wrap = false)
	    : width(w), height(h), wraps(wrap), powerTile({-1, -1})
	{
		grid.resize(h * w);
	}

	Tile &at(int row, int col) { return grid[row * width + col]; }

	const Tile &at(int row, int col) const { return grid[row * width + col]; }
};

struct Move {
  int x;
  int y;
  int rotation;
};

// Implementations

inline uint8_t getActivePortsMask(const Tile &tile)
{
    if (!tile.customConnections.empty()) {
        uint8_t mask = 0;
        int shift = (tile.rotation / 90) % 4;
        for(int i=0; i<4; ++i) {
            if(tile.customConnections[i]) {
                mask |= (1 << ((i + shift) % 4));
            }
        }
        return mask;
    }

	uint8_t baseMask = 0;

	switch (tile.type)
	{
	case EMPTY:
		return 0;

	case POWER:
	case CROSS:
		return 0xF;

	case PC:
		baseMask = 0x1; // NORTH
		break;

	case STRAIGHT:
		baseMask = 0x5; // NORTH | SOUTH (1 | 4)
		break;

	case CORNER:
		baseMask = 0x3; // NORTH | EAST (1 | 2)
		break;

	case T_JUNCTION:
		baseMask = 0x7; // NORTH | EAST | SOUTH (1 | 2 | 4)
		break;
	}

	int rotSteps = (tile.rotation / 90) % 4;
    if (rotSteps == 0) return baseMask;
    
    return ((baseMask << rotSteps) | (baseMask >> (4 - rotSteps))) & 0xF;
}

inline vector<Direction> getActivePorts(const Tile &tile)
{
    uint8_t mask = getActivePortsMask(tile);
    vector<Direction> ports;
    if (mask & 1) ports.push_back(NORTH);
    if (mask & 2) ports.push_back(EAST);
    if (mask & 4) ports.push_back(SOUTH);
    if (mask & 8) ports.push_back(WEST);
    return ports;
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
