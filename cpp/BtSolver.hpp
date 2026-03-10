#ifndef BT_SOLVER_HPP
#define BT_SOLVER_HPP

#include "Tile.hpp"
#include "SolverUtils.hpp"
#include "ConnectivityCheck.hpp"
#include "VisualStep.hpp"
#include <vector>
#include <algorithm>

using namespace std;

class BtSolver {
public:
    explicit BtSolver(Board &b, vector<VisualStep>* s = nullptr) : board(b), steps(s) {}

    bool solve() {
        vector<pair<int, int>> varTiles;
        fixedMap.assign(board.height, vector<bool>(board.width, false));
        for (int r = 0; r < board.height; r++) {
            for (int c = 0; c < board.width; c++) {
                if (board.at(r, c).locked || board.at(r, c).type == EMPTY || board.at(r, c).type == POWER) {
                    fixedMap[r][c] = true;
                } else {
                    varTiles.push_back({r, c});
                }
            }
        }
        
        // Sort tiles for better pruning (heuristic: prioritize highly connected areas)
        sort(varTiles.begin(), varTiles.end(), [&](const pair<int, int> &a, const pair<int, int> &b) {
            return countNeighbors(a.first, a.second, board) > countNeighbors(b.first, b.second, board);
        });

        return solveRecursive(0, varTiles);
    }

private:
    Board &board;
    vector<VisualStep>* steps;
    vector<vector<bool>> fixedMap;

    bool solveRecursive(size_t idx, const vector<pair<int, int>> &tiles) {
        if (idx == tiles.size()) {
            return isSolved(board);
        }

        int r = tiles[idx].first;
        int c = tiles[idx].second;
        vector<int> rotations = getRotationOptions(board.at(r, c));
        int originalRot = board.at(r, c).rotation;

        if (steps && steps->size() < 10000) {
            steps->push_back({r, c, originalRot, "CONSIDER", 0});
        }

        for (int rot : rotations) {
            board.at(r, c).rotation = rot;
            
            if (steps && steps->size() < 10000) {
                steps->push_back({r, c, rot, "TRY", 0});
            }

            if (isPartiallyConsistent(r, c, board)) {
                fixedMap[r][c] = true;
                if (solveRecursive(idx + 1, tiles)) return true;
                fixedMap[r][c] = false;
            }

            if (steps && steps->size() < 10000) {
                steps->push_back({r, c, rot, "UNDO", 0});
            }
        }

        board.at(r, c).rotation = originalRot;
        return false;
    }

    bool isPartiallyConsistent(int r, int c, const Board &b) {
        uint8_t mask = getPortMask(b.at(r, c));
        for (int d = 0; d < 4; d++) {
            Direction dir = static_cast<Direction>(d);
            pair<int, int> nPos = getNeighbor(r, c, dir, b.width, b.height, b.wraps);
            bool myPort = (mask >> d) & 1;

            if (nPos.first == -1) {
                if (myPort) return false;
            } else if (fixedMap[nPos.first][nPos.second]) {
                const Tile &neighbor = b.at(nPos.first, nPos.second);
                uint8_t nMask = getPortMask(neighbor);
                bool nPort = (nMask >> opposite(dir)) & 1;
                if (myPort != nPort) return false;
            }
        }
        return true;
    }

    int countNeighbors(int r, int c, const Board &b) {
        int count = 0;
        for (int d = 0; d < 4; d++) {
            auto n = getNeighbor(r, c, static_cast<Direction>(d), b.width, b.height, b.wraps);
            if (n.first != -1) count++;
        }
        return count;
    }
};

inline bool solve_bt(Board &board, vector<VisualStep>* steps = nullptr) {
    BtSolver solver(board, steps);
    return solver.solve();
}

#endif // BT_SOLVER_HPP
