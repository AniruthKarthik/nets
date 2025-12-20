#ifndef TILE_H
#define TILE_H

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

	Tile() : type(EMPTY), rotation(0) {}
	Tile(TileType t, int r) : type(t), rotation(r) {}
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

vector<Direction> getActivePorts(const Tile &tile);
Direction opposite(Direction dir);
Direction rotateDirection(Direction dir, int rotation);
pair<int, int> getNeighbor(int row, int col, Direction dir, int width,
                           int height, bool wraps);

#endif // TILE_H
