#ifndef DP_SOLVER_HPP
#define DP_SOLVER_HPP

#include "Tile.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {

inline uint8_t portMaskForTile(const Tile &tile, int rotation) {
    if (!tile.customConnections.empty()) {
        uint8_t mask = 0;
        int shift = ((rotation / 90) % 4 + 4) % 4;
        for (int i = 0; i < 4; ++i) {
            if (tile.customConnections[i]) {
                int dir = (i + shift) % 4;
                mask |= (1u << dir);
            }
        }
        return mask;
    }

    static bool initialized = false;
    static uint8_t masks[7][4];
    if (!initialized) {
        for (int t = 0; t < 7; ++t) {
            for (int r = 0; r < 4; ++r) {
                Tile tmp(static_cast<TileType>(t), r * 90, false);
                uint8_t mask = 0;
                vector<Direction> ports = getActivePorts(tmp);
                for (auto d : ports) mask |= (1u << static_cast<int>(d));
                masks[t][r] = mask;
            }
        }
        initialized = true;
    }

    int rot = ((rotation / 90) % 4 + 4) % 4;
    return masks[static_cast<int>(tile.type)][rot];
}

inline vector<int> rotationOptions(const Tile &tile) {
    if (tile.locked) {
        return {tile.rotation};
    }

    if (!tile.customConnections.empty()) {
        if (tile.type == EMPTY) return {0};
        return {0, 90, 180, 270};
    }

    switch (tile.type) {
    case EMPTY:
        return {0};
    case CROSS:
        return {0};
    case STRAIGHT:
        return {0, 90};
    default:
        return {0, 90, 180, 270};
    }
}

inline uint64_t encodeState(uint16_t idx, uint16_t upMask, uint8_t leftReq,
                            uint8_t rowWrapExpected) {
    uint64_t key = idx;
    key |= (static_cast<uint64_t>(upMask) << 16);
    key |= (static_cast<uint64_t>(leftReq & 1u) << 32);
    key |= (static_cast<uint64_t>(rowWrapExpected & 1u) << 33);
    return key;
}

} // namespace

class DpSolver {
public:
    explicit DpSolver(Board &b)
        : board(b), width(b.width), height(b.height), wraps(b.wraps) {}

    bool solve() {
        if (!wraps) {
            memo.clear();
            choice.clear();
            if (!solveFrom(0, 0, 0, 0)) return false;
            applySolution(0, 0, 0, 0);
            return true;
        }

        const uint16_t limit = static_cast<uint16_t>(1u << width);
        for (uint16_t initialUp = 0; initialUp < limit; ++initialUp) {
            memo.clear();
            choice.clear();
            for (uint8_t rowWrap = 0; rowWrap < 2; ++rowWrap) {
                if (solveFrom(0, initialUp, rowWrap, rowWrap, initialUp)) {
                    applySolution(0, initialUp, rowWrap, rowWrap, initialUp);
                    return true;
                }
            }
        }

        return false;
    }

private:
    Board &board;
    int width;
    int height;
    bool wraps;
    unordered_map<uint64_t, bool> memo;
    unordered_map<uint64_t, uint8_t> choice;

    bool solveFrom(uint16_t idx, uint16_t upMask, uint8_t leftReq,
                   uint8_t rowWrapExpected, uint16_t targetUpMask = 0) {
        const uint16_t total = static_cast<uint16_t>(width * height);
        if (idx >= total) {
            if (!wraps) return upMask == 0;
            return upMask == targetUpMask;
        }

        uint64_t key = encodeState(idx, upMask, leftReq, rowWrapExpected);
        auto it = memo.find(key);
        if (it != memo.end()) return it->second;

        int r = idx / width;
        int c = idx % width;

        if (!wraps) {
            if (c == 0 && leftReq != 0) {
                memo[key] = false;
                return false;
            }
        } else {
            if (c == 0 && leftReq != rowWrapExpected) {
                memo[key] = false;
                return false;
            }
        }

        const Tile &tile = board.grid[r][c];
        uint8_t expectedNorth = (upMask >> c) & 1u;
        uint8_t expectedWest = leftReq & 1u;

        vector<int> rotations = rotationOptions(tile);
        for (size_t i = 0; i < rotations.size(); ++i) {
            int rot = rotations[i];
            uint8_t mask = portMaskForTile(tile, rot);

            uint8_t north = (mask >> NORTH) & 1u;
            uint8_t east = (mask >> EAST) & 1u;
            uint8_t south = (mask >> SOUTH) & 1u;
            uint8_t west = (mask >> WEST) & 1u;

            if (north != expectedNorth) continue;
            if (west != expectedWest) continue;

            if (!wraps) {
                if (r == 0 && north != 0) continue;
                if (c == width - 1 && east != 0) continue;
                if (r == height - 1 && south != 0) continue;
            }

            uint16_t nextUpMask = upMask;
            if (south)
                nextUpMask |= (1u << c);
            else
                nextUpMask &= static_cast<uint16_t>(~(1u << c));

            if (c == width - 1) {
                if (wraps) {
                    if (east != rowWrapExpected) continue;
                    if (r == height - 1) {
                        if (solveFrom(static_cast<uint16_t>(idx + 1), nextUpMask, 0, 0,
                                      targetUpMask)) {
                            choice[key] = static_cast<uint8_t>(i);
                            memo[key] = true;
                            return true;
                        }
                    } else {
                        for (uint8_t nextRowWrap = 0; nextRowWrap < 2; ++nextRowWrap) {
                            if (solveFrom(static_cast<uint16_t>(idx + 1), nextUpMask, nextRowWrap,
                                          nextRowWrap, targetUpMask)) {
                                choice[key] = static_cast<uint8_t>(i | (nextRowWrap << 3));
                                memo[key] = true;
                                return true;
                            }
                        }
                    }
                } else {
                    if (east != 0) continue;
                    if (solveFrom(static_cast<uint16_t>(idx + 1), nextUpMask, 0, 0,
                                  targetUpMask)) {
                        choice[key] = static_cast<uint8_t>(i);
                        memo[key] = true;
                        return true;
                    }
                }
            } else {
                uint8_t nextLeftReq = east;
                if (solveFrom(static_cast<uint16_t>(idx + 1), nextUpMask, nextLeftReq,
                              rowWrapExpected, targetUpMask)) {
                    choice[key] = static_cast<uint8_t>(i);
                    memo[key] = true;
                    return true;
                }
            }
        }

        memo[key] = false;
        return false;
    }

    void applySolution(uint16_t idx, uint16_t upMask, uint8_t leftReq,
                       uint8_t rowWrapExpected, uint16_t targetUpMask = 0) {
        const uint16_t total = static_cast<uint16_t>(width * height);
        if (idx >= total) return;

        uint64_t key = encodeState(idx, upMask, leftReq, rowWrapExpected);
        auto it = choice.find(key);
        if (it == choice.end()) return;

        int r = idx / width;
        int c = idx % width;
        Tile &tile = board.grid[r][c];

        vector<int> rotations = rotationOptions(tile);
        uint8_t choiceVal = it->second;
        uint8_t rotIdx = choiceVal & 0x7u;
        if (rotIdx >= rotations.size()) rotIdx = 0;
        int rot = rotations[rotIdx];
        tile.rotation = rot;

        uint8_t mask = portMaskForTile(tile, rot);
        uint8_t east = (mask >> EAST) & 1u;
        uint8_t south = (mask >> SOUTH) & 1u;

        uint16_t nextUpMask = upMask;
        if (south)
            nextUpMask |= (1u << c);
        else
            nextUpMask &= static_cast<uint16_t>(~(1u << c));

        if (c == width - 1) {
            if (wraps && r != height - 1) {
                uint8_t nextRowWrap = (choiceVal >> 3) & 1u;
                applySolution(static_cast<uint16_t>(idx + 1), nextUpMask, nextRowWrap,
                              nextRowWrap, targetUpMask);
            } else {
                applySolution(static_cast<uint16_t>(idx + 1), nextUpMask, 0, 0, targetUpMask);
            }
        } else {
            uint8_t nextLeftReq = east;
            applySolution(static_cast<uint16_t>(idx + 1), nextUpMask, nextLeftReq,
                          rowWrapExpected, targetUpMask);
        }
    }
};

inline bool solve_dp(Board &board) {
    DpSolver solver(board);
    return solver.solve();
}

#endif // DP_SOLVER_HPP
