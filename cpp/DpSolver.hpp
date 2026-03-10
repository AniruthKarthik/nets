#ifndef DP_SOLVER_HPP
#define DP_SOLVER_HPP

#include "Tile.hpp"
#include "SolverUtils.hpp"
#include "VisualStep.hpp"
#include <vector>
#include <unordered_map>
#include <cstdint>

using namespace std;

class DpSolver {
public:
    explicit DpSolver(Board &b, vector<VisualStep>* s = nullptr) : board(b), width(b.width), height(b.height), steps(s) {}

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
            for (uint16_t initialUp = 0; initialUp < (1 << width); ++initialUp) {
                for (uint8_t initialWest = 0; initialWest < 2; ++initialWest) {
                    memo.clear();
                    choice.clear();
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
    vector<VisualStep>* steps;
    unordered_map<uint64_t, bool> memo;
    unordered_map<uint64_t, uint8_t> choice;

    uint64_t encode(int idx, uint16_t upMask, uint8_t leftReq) {
        return (static_cast<uint64_t>(idx) << 17) | (static_cast<uint64_t>(upMask) << 1) | leftReq;
    }

    bool solveRecursive(int idx, uint16_t upMask, uint8_t leftReq, uint8_t rowWrapTarget, uint16_t colWrapTarget) {
        if (idx == width * height) {
            return (upMask == colWrapTarget);
        }

        uint64_t key = encode(idx, upMask, leftReq);
        if (memo.count(key)) return memo[key];

        int r = idx / width;
        int c = idx % width;

        const Tile &tile = board.at(r, c);
        uint8_t reqNorth = (upMask >> c) & 1;
        uint8_t reqWest = leftReq;

        if (steps && steps->size() < 5000) {
            VisualStep s;
            s.row = r; s.col = c; s.rotation = tile.rotation;
            s.type = "CONSIDER";
            s.upmask = upMask;
            s.leftreq = leftReq;
            steps->push_back(s);
        }

        vector<int> rotations = getRotationOptions(tile);
        for (size_t i = 0; i < rotations.size(); ++i) {
            int rot = rotations[i];
            Tile tempTile = tile;
            tempTile.rotation = rot;
            uint8_t mask = getPortMask(tempTile);

            uint8_t n = (mask >> NORTH) & 1;
            uint8_t e = (mask >> EAST) & 1;
            uint8_t s = (mask >> SOUTH) & 1;
            uint8_t w = (mask >> WEST) & 1;

            if (n != reqNorth || w != reqWest) continue;

            if (!board.wraps) {
                if (r == 0 && n != 0) continue;
                if (r == height - 1 && s != 0) continue;
                if (c == 0 && w != 0) continue;
                if (c == width - 1 && e != 0) continue;
            }

            if (steps && steps->size() < 5000) {
                steps->push_back({r, c, rot, "TRY", 0, (int)upMask, (int)leftReq});
            }

            uint16_t nextUpMask = upMask;
            if (s) nextUpMask |= (1 << c);
            else nextUpMask &= ~(1 << c);

            uint8_t nextLeftReq;
            if (c == width - 1) {
                nextLeftReq = board.wraps ? rowWrapTarget : 0;
            } else {
                nextLeftReq = e;
            }

            if (solveRecursive(idx + 1, nextUpMask, nextLeftReq, rowWrapTarget, colWrapTarget)) {
                choice[key] = (uint8_t)i;
                return memo[key] = true;
            }

            if (steps && steps->size() < 5000) {
                steps->push_back({r, c, rot, "UNDO", 0, (int)upMask, (int)leftReq});
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
        vector<int> rotations = getRotationOptions(board.at(r, c));
        board.at(r, c).rotation = rotations[rotIdx];

        uint8_t mask = getPortMask(board.at(r, c));
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

inline bool solve_dp(Board &board, vector<VisualStep>* steps = nullptr) {
    DpSolver solver(board, steps);
    return solver.solve();
}

#endif // DP_SOLVER_HPP
