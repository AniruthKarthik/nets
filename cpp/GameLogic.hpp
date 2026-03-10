#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "Tile.hpp"
#include <vector>
#include <utility>

using namespace std;

// Time Complexity: $O(1)$
// Space Complexity: $O(1)$
inline Direction opposite(Direction d)
{
	switch (d)
	{
	case NORTH: return SOUTH;
	case SOUTH: return NORTH;
	case EAST:  return WEST;
	case WEST:  return EAST;
	default:    return NORTH;
	}
}

// Time Complexity: $O(1)$
// Space Complexity: $O(1)$
inline pair<int, int> getNeighbor(int r, int c, Direction d, int w, int h, bool wrap)
{
	int nr = r, nc = c;
	switch (d)
	{
	case NORTH: nr--; break;
	case SOUTH: nr++; break;
	case EAST:  nc++; break;
	case WEST:  nc--; break;
	}

	if (wrap)
	{
		nr = (nr + h) % h;
		nc = (nc + w) % w;
		return {nr, nc};
	}
	else
	{
		if (nr < 0 || nr >= h || nc < 0 || nc >= w)
			return {-1, -1};
		return {nr, nc};
	}
}

// Time Complexity: $O(1)$
// Space Complexity: $O(1)$
inline void applyMove(Board &board, const Move &move)
{
	board.at(move.x, move.y).rotation = move.rotation;
}

#endif // GAME_LOGIC_HPP
