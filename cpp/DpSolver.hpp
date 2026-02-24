#ifndef DP_SOLVER_HPP
#define DP_SOLVER_HPP

#include "Tile.hpp"
#include <vector>
#include <cstdint>

using namespace std;

inline uint8_t portMaskFromTile_dp(const Tile &tile) {
    if (!tile.customConnections.empty()) {
        uint8_t mask = 0;
        vector<Direction> ports = getActivePorts(tile);
        for (auto d : ports) mask |= (1u << static_cast<int>(d));
        return mask;
    }

    static bool initialized = false;
    static uint8_t masks[7][4];
    if (!initialized) {
        for (int t = 0; t < 7; t++) {
            for (int r = 0; r < 4; r++) {
                Tile tmp(static_cast<TileType>(t), r * 90, false);
                uint8_t mask = 0;
                vector<Direction> ports = getActivePorts(tmp);
                for (auto d : ports) mask |= (1u << static_cast<int>(d));
                masks[t][r] = mask;
            }
        }
        initialized = true;
    }

    int rot = ((tile.rotation / 90) % 4 + 4) % 4;
    return masks[static_cast<int>(tile.type)][rot];
}

inline bool checkConsistency_dp(const Board &board, int row, int col,
                                const vector<vector<bool>> &fixedMap) {
    const Tile &tile = board.at(row, col);
    uint8_t mustConnect = 0;
    uint8_t mustNotConnect = 0;

    for (int d = 0; d < 4; d++) {
        Direction dir = static_cast<Direction>(d);
        pair<int, int> nPos = getNeighbor(row, col, dir, board.width, board.height, board.wraps);

        if (nPos.first == -1) {
            mustNotConnect |= (1u << d);
            continue;
        }

        if (fixedMap[nPos.first][nPos.second]) {
            const Tile &nTile = board.at(nPos.first, nPos.second);
            uint8_t nMask = portMaskFromTile_dp(nTile);
            bool nConn = (nMask & (1u << static_cast<int>(opposite(dir)))) != 0;
            if (nConn) {
                mustConnect |= (1u << d);
            } else {
                mustNotConnect |= (1u << d);
            }
        }
    }

    uint8_t myMask = portMaskFromTile_dp(tile);
    if ((myMask & mustNotConnect) != 0) return false;
    if ((myMask & mustConnect) != mustConnect) return false;
    return true;
}

inline bool solveRecursive_dp(int idx, const vector<pair<int, int>> &tiles,
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
        if (checkConsistency_dp(board, r, c, fixedMap)) {
            fixedMap[r][c] = true;
            if (solveRecursive_dp(idx + 1, tiles, board, fixedMap)) return true;
            fixedMap[r][c] = false;
        }
    }
    board.grid[r][c].rotation = originalRot;
    return false;
}

inline bool solve_dp(Board &board) {
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

    return solveRecursive_dp(0, tilesToSolve, board, fixedMap);
}

#endif // DP_SOLVER_HPP
