#ifndef SOLVER_UTILS_HPP
#define SOLVER_UTILS_HPP

#include "Tile.hpp"
#include <vector>
#include <cstdint>

using namespace std;

inline uint8_t getPortMask(const Tile &tile) {
    uint8_t mask = 0;
    if (!tile.customConnections.empty()) {
        int shift = (tile.rotation / 90) % 4;
        for (int i = 0; i < 4; i++) {
            if (tile.customConnections[i]) {
                mask |= (1 << ((i + shift) % 4));
            }
        }
        return mask;
    }

    // Default tile types
    switch (tile.type) {
        case EMPTY: return 0;
        case POWER:
        case CROSS: return 0xF; // 1111
        case PC: {
            int rot = (tile.rotation / 90) % 4;
            return (1 << rot);
        }
        case STRAIGHT: {
            int rot = (tile.rotation / 90) % 2;
            if (rot == 0) return 0x5; // 0101 (N, S)
            return 0xA; // 1010 (E, W)
        }
        case CORNER: {
            int rot = (tile.rotation / 90) % 4;
            // 0: N,E (3), 1: E,S (6), 2: S,W (12), 3: W,N (9)
            uint8_t base = 0x3; // N, E
            return ((base << rot) | (base >> (4 - rot))) & 0xF;
        }
        case T_JUNCTION: {
            int rot = (tile.rotation / 90) % 4;
            // 0: N,E,S (7), 1: E,S,W (14), 2: S,W,N (13), 3: W,N,E (11)
            uint8_t base = 0x7; // N, E, S
            return ((base << rot) | (base >> (4 - rot))) & 0xF;
        }
        default: return 0;
    }
}

inline vector<int> getRotationOptions(const Tile &tile) {
    if (tile.locked) return {tile.rotation};
    if (tile.type == EMPTY || tile.type == CROSS) return {0};
    if (tile.type == STRAIGHT) return {0, 90};
    return {0, 90, 180, 270};
}

// Check if a rotation of a tile is consistent with its neighbors
inline bool isCompatible(uint8_t mask, int r, int c, const Board &board, const vector<vector<bool>> &fixedMap) {
    for (int d = 0; d < 4; d++) {
        Direction dir = static_cast<Direction>(d);
        pair<int, int> nPos = getNeighbor(r, c, dir, board.width, board.height, board.wraps);
        bool myPort = (mask >> d) & 1;

        if (nPos.first == -1) {
            // Board edge (no wrap)
            if (myPort) return false;
        } else if (fixedMap[nPos.first][nPos.second]) {
            // Fixed neighbor
            uint8_t nMask = getPortMask(board.at(nPos.first, nPos.second));
            bool nPort = (nMask >> opposite(dir)) & 1;
            if (myPort != nPort) return false;
        }
    }
    return true;
}

#endif // SOLVER_UTILS_HPP
