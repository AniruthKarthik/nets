#ifndef DP_SOLVER_HPP
#define DP_SOLVER_HPP

#include "Tile.hpp"
#include "SolverUtils.hpp"
#include <vector>
#include <unordered_map>
#include <cstdint>

using namespace std;

class DpSolver {
public:
    explicit DpSolver(Board &b) : board(b), width(b.width), height(b.height) {}

    bool solve() {
        if (width > 16) return false; // upMask limit

        if (!board.wraps) {
            memo.clear();
            choice.clear();
            if (solveRecursive(0, 0, 0, 0, 0)) {
                applySolution(0, 0, 0, 0, 0);
                return true;
            }
        } else {
            // Brute force the wrap-around constraints
            // initialUp: North connections for row 0 (which are south connections from row height-1)
            // initialWest: West connections for col 0 (which are east connections from col width-1)
            for (uint16_t initialUp = 0; initialUp < (1 << width); ++initialUp) {
                for (uint8_t initialWest = 0; initialWest < 2; ++initialWest) {
                    memo.clear();
                    choice.clear();
                    // We need to match these at the end
                    if (solveRecursive(0, initialUp, initialWest, initialWest, initialUp)) {
                        applySolution(0, initialUp, initialWest, initialWest, initialUp);
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:
    Board &board;
    int width, height;
    unordered_map<uint64_t, bool> memo;
    unordered_map<uint64_t, uint8_t> choice;

    // State encoding: index (16 bits) | upMask (16 bits) | leftReq (1 bit)
    uint64_t encode(int idx, uint16_t upMask, uint8_t leftReq) {
        return (static_cast<uint64_t>(idx) << 17) | (static_cast<uint64_t>(upMask) << 1) | leftReq;
    }

    bool solveRecursive(int idx, uint16_t upMask, uint8_t leftReq, uint8_t rowWrapTarget, uint16_t colWrapTarget) {
        if (idx == width * height) {
            return (upMask == colWrapTarget); // All south connections from last row must match colWrapTarget
        }

        uint64_t key = encode(idx, upMask, leftReq);
        if (memo.count(key)) return memo[key];

        int r = idx / width;
        int c = idx % width;

        const Tile &tile = board.grid[r][c];
        uint8_t reqNorth = (upMask >> c) & 1;
        uint8_t reqWest = leftReq;

        vector<int> rotations = getRotationOptions(tile);
        for (size_t i = 0; i < rotations.size(); ++i) {
            int rot = rotations[i];
            uint8_t mask = getPortMask(Tile(tile.type, rot, false)); // Use temp tile for custom conns check if needed
            // Wait, getPortMask already handles customConnections if we pass the tile.
            // But we need to set the rotation temporarily.
            Tile tempTile = tile;
            tempTile.rotation = rot;
            mask = getPortMask(tempTile);

            uint8_t n = (mask >> NORTH) & 1;
            uint8_t e = (mask >> EAST) & 1;
            uint8_t s = (mask >> SOUTH) & 1;
            uint8_t w = (mask >> WEST) & 1;

            if (n != reqNorth || w != reqWest) continue;

            // Boundary checks (no wrap)
            if (!board.wraps) {
                if (r == 0 && n != 0) continue;
                if (r == height - 1 && s != 0) continue;
                if (c == 0 && w != 0) continue;
                if (c == width - 1 && e != 0) continue;
            }

            uint16_t nextUpMask = upMask;
            if (s) nextUpMask |= (1 << c);
            else nextUpMask &= ~(1 << c);

            uint8_t nextLeftReq;
            uint8_t nextRowWrapTarget = rowWrapTarget;
            if (c == width - 1) {
                if (board.wraps && e != rowWrapTarget) continue;
                if (!board.wraps && e != 0) continue;
                
                if (r == height - 1) {
                    nextLeftReq = 0; // Doesn't matter, will exit
                } else {
                    // Start next row. Need to decide next row's wrap target if we were brute-forcing it differently?
                    // Actually, rowWrapTarget is constant for the whole solve attempt.
                    nextLeftReq = rowWrapTarget; 
                }
            } else {
                nextLeftReq = e;
            }

            if (solveRecursive(idx + 1, nextUpMask, nextLeftReq, nextRowWrapTarget, colWrapTarget)) {
                choice[key] = (uint8_t)i;
                return memo[key] = true;
            }
        }

        return memo[key] = false;
    }

    void applySolution(int idx, uint16_t upMask, uint8_t leftReq, uint8_t rowWrapTarget, uint16_t colWrapTarget) {
        if (idx == width * height) return;
        uint64_t key = encode(idx, upMask, leftReq);
        int rotIdx = choice[key];
        
        int r = idx / width;
        int c = idx % width;
        vector<int> rotations = getRotationOptions(board.grid[r][c]);
        board.grid[r][c].rotation = rotations[rotIdx];

        uint8_t mask = getPortMask(board.grid[r][c]);
        uint8_t s = (mask >> SOUTH) & 1;
        uint8_t e = (mask >> EAST) & 1;

        uint16_t nextUpMask = upMask;
        if (s) nextUpMask |= (1 << c);
        else nextUpMask &= ~(1 << c);

        uint8_t nextLeftReq;
        if (c == width - 1) {
            nextLeftReq = rowWrapTarget;
        } else {
            nextLeftReq = e;
        }

        applySolution(idx + 1, nextUpMask, nextLeftReq, rowWrapTarget, colWrapTarget);
    }
};

inline bool solve_dp(Board &board) {
    DpSolver solver(board);
    return solver.solve();
}

#endif // DP_SOLVER_HPP
