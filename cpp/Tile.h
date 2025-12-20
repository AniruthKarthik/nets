#ifndef TILE_H
#define TILE_H

#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace std;

// Directions
enum Direction
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

// Tile types
enum TileType
{
	EMPTY,      // No connections (shouldn't exist in valid puzzles)
	POWER,      // Power source (4 ports)
	PC,         // Computer endpoint (1 port)
	STRAIGHT,   // Line: 2 opposite ports
	CORNER,     // L-shape: 2 adjacent ports
	T_JUNCTION, // T-shape: 3 ports
	CROSS       // +: 4 ports
};

// Tile structure
struct Tile
{
	TileType type;
	int rotation; // 0, 90, 180, 270 degrees

	Tile() : type(EMPTY), rotation(0) {}
	Tile(TileType t, int r) : type(t), rotation(r) {}
};

// Board structure
struct Board
{
	int width;
	int height;
	bool wraps; // Does the grid wrap around?
	vector<vector<Tile>> grid;
	pair<int, int> powerTile; // Location of power source

	Board(int w, int h, bool wrap = false)
	    : width(w), height(h), wraps(wrap), powerTile({-1, -1})
	{
		grid.resize(h, vector<Tile>(w));
	}

	Tile &at(int row, int col) { return grid[row][col]; }

	const Tile &at(int row, int col) const { return grid[row][col]; }
};

// Get active ports for a tile considering its rotation
vector<Direction> getActivePorts(const Tile &tile);

// Get opposite direction
Direction opposite(Direction dir);

// Helper to rotate direction
Direction rotateDirection(Direction dir, int rotation);

#endif
