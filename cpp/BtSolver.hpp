#ifndef BT_SOLVER_HPP
#define BT_SOLVER_HPP

#include "Tile.hpp"
#include "SolverUtils.hpp"
#include "SortUtils.hpp"
#include <vector>

using namespace std;

class BtSolver {
public:
    explicit BtSolver(Board &b) : board(b) {
        fixedMap.assign(board.height, vector<bool>(board.width, false));
    }

    bool solve() {
        vector<pair<int, int>> tilesToSolve;
        for (int r = 0; r < board.height; r++) {
            for (int c = 0; c < board.width; c++) {
                if (board.grid[r][c].locked || board.grid[r][c].type == EMPTY) {
                    fixedMap[r][c] = true;
                } else {
                    tilesToSolve.push_back({r, c});
                }
            }
        }

        // Use heuristic sorting from SortUtils
        sortTiles_dac(tilesToSolve, board);

        return solveRecursive(0, tilesToSolve);
    }

private:
    Board &board;
    vector<vector<bool>> fixedMap;

    bool solveRecursive(size_t idx, const vector<pair<int, int>> &tiles) {
        if (idx == tiles.size()) return true;

        int r = tiles[idx].first;
        int c = tiles[idx].second;
        
        vector<int> rotations = getRotationOptions(board.grid[r][c]);
        int originalRot = board.grid[r][c].rotation;

        for (int rot : rotations) {
            board.grid[r][c].rotation = rot;
            uint8_t mask = getPortMask(board.grid[r][c]);

            if (isCompatible(mask, r, c, board, fixedMap)) {
                fixedMap[r][c] = true;
                if (solveRecursive(idx + 1, tiles)) return true;
                fixedMap[r][c] = false;
            }
        }

        board.grid[r][c].rotation = originalRot;
        return false;
    }
};

inline bool solve_bt(Board &board) {
    BtSolver solver(board);
    return solver.solve();
}

#endif // BT_SOLVER_HPP
