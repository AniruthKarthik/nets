#ifndef BACKTRACKING_SOLVER_HPP
#define BACKTRACKING_SOLVER_HPP

#include "SortUtils.hpp"
#include "Tile.hpp"
#include "GameLogic.hpp"
#include <vector>

using namespace std;

// Implementations

inline bool checkConsistency(const Board &board, int row, int col, const vector<vector<bool>> &fixedMap) {
    const Tile &tile = board.at(row, col);
    vector<Direction> myPorts = getActivePorts(tile);
    bool myConns[4] = {false, false, false, false};
    for (auto d : myPorts) myConns[d] = true;

    for (int d = 0; d < 4; d++) {
        Direction dir = static_cast<Direction>(d);
        pair<int, int> nPos = getNeighbor(row, col, dir, board.width, board.height, board.wraps);

        if (nPos.first == -1) {
            // Board Edge (no wrap)
            // If I have a connection pointing off-board, that's invalid.
            if (myConns[d]) return false;
        } else {
            if (fixedMap[nPos.first][nPos.second]) {
                const Tile &nTile = board.at(nPos.first, nPos.second);
                bool nConn = false;
                if (nTile.type != EMPTY) {
                    vector<Direction> nPorts = getActivePorts(nTile);
                    Direction opp = opposite(dir);
                    for (auto nd : nPorts) {
                        if (nd == opp) {
                            nConn = true;
                            break;
                        }
                    }
                }
                
                // If neighbor is fixed, we MUST match.
                // Connected <-> Connected
                // Not Connected <-> Not Connected
                if (myConns[d] != nConn) {
                    return false;
                }
            }
        }
    }
    return true;
}

inline bool solveRecursive(int idx, const vector<pair<int, int>> &tiles, Board &board, vector<vector<bool>> &fixedMap) {
    if (idx >= tiles.size()) {
        return true; // All tiles placed successfully
    }

    int r = tiles[idx].first;
    int c = tiles[idx].second;

    // Try all 4 rotations
    // Optimization: If tile is symmetric, skip redundant rotations
    int maxRotations = 4;
    TileType type = board.grid[r][c].type;
    if (type == STRAIGHT) maxRotations = 2; // 0 and 90 are distinct. 180 is same as 0.
    if (type == CROSS) maxRotations = 1;    // Symmetric
    // EMPTY is handled by not being in the list usually, but if it is:
    if (type == EMPTY) maxRotations = 1;

    for (int rot = 0; rot < maxRotations; rot++) {
        int actualRot = rot * 90;
        board.grid[r][c].rotation = actualRot;

        if (checkConsistency(board, r, c, fixedMap)) {
            fixedMap[r][c] = true;
            if (solveRecursive(idx + 1, tiles, board, fixedMap)) {
                return true;
            }
            fixedMap[r][c] = false; // Backtrack
        }
    }

    return false;
}

inline bool solveBacktracking_dac(Board &board) {
    vector<pair<int, int>> tilesToSort;
    vector<vector<bool>> fixedMap(board.height, vector<bool>(board.width, false));

    // Identify variable tiles and fixed tiles
    for (int r = 0; r < board.height; r++) {
        for (int c = 0; c < board.width; c++) {
            if (board.grid[r][c].locked) {
                fixedMap[r][c] = true;
            } else if (board.grid[r][c].type != EMPTY) {
                tilesToSort.push_back({r, c});
            } else {
                // EMPTY tiles are effectively fixed (no connections)
                fixedMap[r][c] = true;
            }
        }
    }

    // DIVIDE AND CONQUER: Sort the tiles to optimize backtracking
    sortTiles_dac(tilesToSort, board);

    return solveRecursive(0, tilesToSort, board, fixedMap);
}

#endif // BACKTRACKING_SOLVER_HPP
