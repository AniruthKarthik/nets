#ifndef DAC_SOLVER_HPP
#define DAC_SOLVER_HPP

#include "SortUtils.hpp"
#include "Tile.hpp"
#include "GameLogic.hpp"
#include <vector>
#include <unordered_map>
#include <functional>

using namespace std;

// Implementations

struct Region_dac {
    int r0;
    int r1;
    int c0;
    int c1;
};

inline bool inRegion_dac(const Region_dac &region, int r, int c) {
    return r >= region.r0 && r < region.r1 && c >= region.c0 && c < region.c1;
}

inline long long makeEdgeKey_dac(int r1, int c1, int r2, int c2) {
    if (r1 > r2 || (r1 == r2 && c1 > c2)) {
        std::swap(r1, r2);
        std::swap(c1, c2);
    }
    long long key = 0;
    key |= (static_cast<long long>(r1) & 0xFFFF) << 48;
    key |= (static_cast<long long>(c1) & 0xFFFF) << 32;
    key |= (static_cast<long long>(r2) & 0xFFFF) << 16;
    key |= (static_cast<long long>(c2) & 0xFFFF);
    return key;
}

inline bool tileHasConn_dac(const Tile &tile, Direction dir) {
    if (tile.type == EMPTY) return false;
    vector<Direction> ports = getActivePorts(tile);
    for (auto d : ports) {
        if (d == dir) return true;
    }
    return false;
}

inline bool checkConsistency_dac(const Board &board, int row, int col,
                                 const vector<vector<bool>> &fixedMap,
                                 const Region_dac &region,
                                 const unordered_map<long long, bool> &edgeConstraints) {
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
            // Board Edge (no wrap)
            if (myConns[d]) return false;
        } else {
            bool neighborInRegion = inRegion_dac(region, nPos.first, nPos.second);

            if (neighborInRegion && fixedMap[nPos.first][nPos.second]) {
                const Tile &nTile = board.at(nPos.first, nPos.second);
                bool nConn = tileHasConn_dac(nTile, opposite(dir));
                if (myConns[d] != nConn) {
                    return false;
                }
            } else if (!neighborInRegion) {
                const Tile &nTile = board.at(nPos.first, nPos.second);
                bool neighborFixed = nTile.locked || nTile.type == EMPTY;

                if (neighborFixed) {
                    bool nConn = tileHasConn_dac(nTile, opposite(dir));
                    if (myConns[d] != nConn) {
                        return false;
                    }
                }

                long long edgeKey = makeEdgeKey_dac(row, col, nPos.first, nPos.second);
                auto it = edgeConstraints.find(edgeKey);
                if (it != edgeConstraints.end()) {
                    if (myConns[d] != it->second) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

inline bool validateFixedTiles_dac(const Board &board,
                                   const Region_dac &region,
                                   const unordered_map<long long, bool> &edgeConstraints,
                                   const vector<vector<bool>> &fixedMap) {
    for (int r = region.r0; r < region.r1; r++) {
        for (int c = region.c0; c < region.c1; c++) {
            if (fixedMap[r][c]) {
                if (!checkConsistency_dac(board, r, c, fixedMap, region, edgeConstraints)) {
                    return false;
                }
            }
        }
    }
    return true;
}

inline bool solveRegionEnumerate_dac(
    int idx,
    const vector<pair<int, int>> &tiles,
    Board &board,
    vector<vector<bool>> &fixedMap,
    const Region_dac &region,
    const unordered_map<long long, bool> &edgeConstraints,
    const function<bool()> &onSolution) {
    if (idx >= static_cast<int>(tiles.size())) {
        return onSolution();
    }

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
        if (checkConsistency_dac(board, r, c, fixedMap, region, edgeConstraints)) {
            fixedMap[r][c] = true;
            if (solveRegionEnumerate_dac(idx + 1, tiles, board, fixedMap, region, edgeConstraints, onSolution)) {
                return true;
            }
            fixedMap[r][c] = false;
        }
    }

    board.grid[r][c].rotation = originalRot;
    return false;
}

inline int countVariableTiles_dac(const Board &board, const Region_dac &region) {
    int count = 0;
    for (int r = region.r0; r < region.r1; r++) {
        for (int c = region.c0; c < region.c1; c++) {
            const Tile &t = board.at(r, c);
            if (!t.locked && t.type != EMPTY) count++;
        }
    }
    return count;
}

inline bool solveRegionWithCallback_dac(const Region_dac &region,
                                        const unordered_map<long long, bool> &edgeConstraints,
                                        Board &board,
                                        const function<bool()> &onSolution) {
    int width = region.c1 - region.c0;
    int height = region.r1 - region.r0;
    int varCount = countVariableTiles_dac(board, region);

    const int leafThreshold = 12;
    if (varCount == 0) {
        vector<vector<bool>> fixedMap(board.height, vector<bool>(board.width, false));
        for (int r = region.r0; r < region.r1; r++) {
            for (int c = region.c0; c < region.c1; c++) {
                const Tile &t = board.at(r, c);
                if (t.locked || t.type == EMPTY) fixedMap[r][c] = true;
            }
        }
        if (!validateFixedTiles_dac(board, region, edgeConstraints, fixedMap)) return false;
        return onSolution();
    }

    if (varCount <= leafThreshold || width == 1 || height == 1) {
        vector<pair<int, int>> tilesToSolve;
        vector<vector<bool>> fixedMap(board.height, vector<bool>(board.width, false));
        for (int r = region.r0; r < region.r1; r++) {
            for (int c = region.c0; c < region.c1; c++) {
                const Tile &t = board.at(r, c);
                if (t.locked || t.type == EMPTY) {
                    fixedMap[r][c] = true;
                } else {
                    tilesToSolve.push_back({r, c});
                }
            }
        }

        if (!validateFixedTiles_dac(board, region, edgeConstraints, fixedMap)) return false;
        sortTiles_dac(tilesToSolve, board);
        return solveRegionEnumerate_dac(0, tilesToSolve, board, fixedMap, region, edgeConstraints, onSolution);
    }

    bool splitVertical = (width >= height);
    if (splitVertical) {
        int mid = region.c0 + width / 2;
        Region_dac left = {region.r0, region.r1, region.c0, mid};
        Region_dac right = {region.r0, region.r1, mid, region.c1};

        auto leftCallback = [&](void) -> bool {
            unordered_map<long long, bool> newConstraints = edgeConstraints;

            for (int r = region.r0; r < region.r1; r++) {
                int lc = mid - 1;
                int rc = mid;
                if (lc < left.c0 || rc >= right.c1) continue;
                const Tile &leftTile = board.at(r, lc);
                bool leftConn = tileHasConn_dac(leftTile, EAST);
                long long edgeKey = makeEdgeKey_dac(r, lc, r, rc);
                auto it = newConstraints.find(edgeKey);
                if (it != newConstraints.end() && it->second != leftConn) {
                    return false;
                }
                newConstraints[edgeKey] = leftConn;
            }

            return solveRegionWithCallback_dac(right, newConstraints, board, onSolution);
        };

        return solveRegionWithCallback_dac(left, edgeConstraints, board, leftCallback);
    } else {
        int mid = region.r0 + height / 2;
        Region_dac top = {region.r0, mid, region.c0, region.c1};
        Region_dac bottom = {mid, region.r1, region.c0, region.c1};

        auto topCallback = [&](void) -> bool {
            unordered_map<long long, bool> newConstraints = edgeConstraints;

            for (int c = region.c0; c < region.c1; c++) {
                int tr = mid - 1;
                int br = mid;
                if (tr < top.r0 || br >= bottom.r1) continue;
                const Tile &topTile = board.at(tr, c);
                bool topConn = tileHasConn_dac(topTile, SOUTH);
                long long edgeKey = makeEdgeKey_dac(tr, c, br, c);
                auto it = newConstraints.find(edgeKey);
                if (it != newConstraints.end() && it->second != topConn) {
                    return false;
                }
                newConstraints[edgeKey] = topConn;
            }

            return solveRegionWithCallback_dac(bottom, newConstraints, board, onSolution);
        };

        return solveRegionWithCallback_dac(top, edgeConstraints, board, topCallback);
    }
}

inline bool solve_dac(Board &board) {
    Region_dac whole = {0, board.height, 0, board.width};
    unordered_map<long long, bool> edgeConstraints;
    return solveRegionWithCallback_dac(whole, edgeConstraints, board, []() { return true; });
}

#endif // DAC_SOLVER_HPP
