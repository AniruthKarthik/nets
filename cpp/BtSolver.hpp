#ifndef BT_SOLVER_HPP
#define BT_SOLVER_HPP

#include "Tile.hpp"
#include <vector>

using namespace std;

inline bool checkConsistency_bt(const Board &board, int row, int col,
                                const vector<vector<bool>> &fixedMap) {
    const Tile &tile = board.at(row, col);
    bool myConns[4] = {false, false, false, false};
    if (tile.type != EMPTY) {
        vector<Direction> myPorts = getActivePorts(tile);
        for (auto d : myPorts) myConns[d] = true;
    }

    for (int d = 0; d < 4; d++) {
        Direction dir = static_cast<Direction>(d);
        pair<int, int> nPos = getNeighbor(row, col, dir, board.width, board.height, board.wraps);

        if (nPos.first == -1) {
            if (myConns[d]) return false;
        } else if (fixedMap[nPos.first][nPos.second]) {
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
            if (myConns[d] != nConn) return false;
        }
    }
    return true;
}

inline bool solveRecursive_bt(int idx, const vector<pair<int, int>> &tiles,
                              Board &board, vector<vector<bool>> &fixedMap) {
    if (idx >= static_cast<int>(tiles.size())) return true;

    int r = tiles[idx].first;
    int c = tiles[idx].second;
    TileType type = board.grid[r][c].type;

    int maxRotations = 4;
    if (board.grid[r][c].customConnections.empty()) {
        if (type == STRAIGHT) maxRotations = 2;
        if (type == CROSS) maxRotations = 1;
        if (type == EMPTY) maxRotations = 1;
    } else {
        if (type == EMPTY) maxRotations = 1;
    }

    int originalRot = board.grid[r][c].rotation;
    for (int rot = 0; rot < maxRotations; rot++) {
        board.grid[r][c].rotation = rot * 90;
        if (checkConsistency_bt(board, r, c, fixedMap)) {
            fixedMap[r][c] = true;
            if (solveRecursive_bt(idx + 1, tiles, board, fixedMap)) return true;
            fixedMap[r][c] = false;
        }
    }
    board.grid[r][c].rotation = originalRot;
    return false;
}

inline bool solve_bt(Board &board) {
    vector<pair<int, int>> tilesToSolve;
    vector<vector<bool>> fixedMap(board.height, vector<bool>(board.width, false));

    for (int r = 0; r < board.height; r++) {
        for (int c = 0; c < board.width; c++) {
            if (board.grid[r][c].locked || board.grid[r][c].type == EMPTY) {
                fixedMap[r][c] = true;
            } else {
                tilesToSolve.push_back({r, c});
            }
        }
    }

    return solveRecursive_bt(0, tilesToSolve, board, fixedMap);
}

#endif // BT_SOLVER_HPP
