#ifndef DAC_SOLVER_HPP
#define DAC_SOLVER_HPP

#include "Tile.hpp"
#include "SolverUtils.hpp"
#include "SortUtils.hpp"
#include "VisualStep.hpp"
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

using namespace std;

struct Region {
    int r0, r1, c0, c1;
    int width() const { return c1 - c0; }
    int height() const { return r1 - r0; }
};

class DacSolver {
public:
    explicit DacSolver(Board &b, vector<VisualStep>* s = nullptr) : board(b), steps(s) {}

    bool solve() {
        fixedMap.assign(board.height, vector<bool>(board.width, false));
        for (int r = 0; r < board.height; r++) {
            for (int c = 0; c < board.width; c++) {
                if (board.at(r, c).locked || board.at(r, c).type == EMPTY) {
                    fixedMap[r][c] = true;
                }
            }
        }
        Region whole = {0, board.height, 0, board.width};
        return solveRegion(whole, {});
    }

private:
    Board &board;
    vector<VisualStep>* steps;
    vector<vector<bool>> fixedMap;

    struct EdgeKey {
        int r1, c1, r2, c2;
        bool operator==(const EdgeKey &other) const {
            return r1 == other.r1 && c1 == other.c1 && r2 == other.r2 && c2 == other.c2;
        }
    };

    struct EdgeKeyHash {
        size_t operator()(const EdgeKey &k) const {
            return (k.r1 << 24) ^ (k.c1 << 16) ^ (k.r2 << 8) ^ k.c2;
        }
    };

    using Constraints = unordered_map<EdgeKey, bool, EdgeKeyHash>;

    EdgeKey makeKey(int r1, int c1, int r2, int c2) {
        if (r1 > r2 || (r1 == r2 && c1 > c2)) return {r2, c2, r1, c1};
        return {r1, c1, r2, c2};
    }

    bool isConsistent(int r, int c, const Region &reg, const Constraints &constraints) {
        uint8_t mask = getPortMask(board.at(r, c));
        for (int d = 0; d < 4; d++) {
            Direction dir = static_cast<Direction>(d);
            pair<int, int> nPos = getNeighbor(r, c, dir, board.width, board.height, board.wraps);
            bool myPort = (mask >> d) & 1;

            if (nPos.first == -1) {
                if (myPort) return false;
            } else {
                bool nInRegion = (nPos.first >= reg.r0 && nPos.first < reg.r1 && nPos.second >= reg.c0 && nPos.second < reg.c1);
                if (nInRegion && fixedMap[nPos.first][nPos.second]) {
                    uint8_t nMask = getPortMask(board.at(nPos.first, nPos.second));
                    if (myPort != ((nMask >> opposite(dir)) & 1)) return false;
                } else if (!nInRegion) {
                    EdgeKey key = makeKey(r, c, nPos.first, nPos.second);
                    if (constraints.count(key) && constraints.at(key) != myPort) return false;
                    if (fixedMap[nPos.first][nPos.second]) {
                        uint8_t nMask = getPortMask(board.at(nPos.first, nPos.second));
                        if (myPort != ((nMask >> opposite(dir)) & 1)) return false;
                    }
                }
            }
        }
        return true;
    }

    bool solveLeaf(size_t idx, const vector<pair<int, int>> &tiles, const Region &reg, const Constraints &constraints) {
        if (idx == tiles.size()) return true;

        int r = tiles[idx].first;
        int c = tiles[idx].second;
        vector<int> rotations = getRotationOptions(board.at(r, c));
        int orig = board.at(r, c).rotation;

        if (steps && steps->size() < 10000) {
            VisualStep s;
            s.row = r; s.col = c; s.rotation = orig; s.type = "CONSIDER";
            s.r0 = reg.r0; s.r1 = reg.r1; s.c0 = reg.c0; s.c1 = reg.c1;
            steps->push_back(s);
        }

        for (int rot : rotations) {
            board.at(r, c).rotation = rot;
            
            if (steps && steps->size() < 10000) {
                VisualStep s;
                s.row = r; s.col = c; s.rotation = rot; s.type = "TRY";
                s.r0 = reg.r0; s.r1 = reg.r1; s.c0 = reg.c0; s.c1 = reg.c1;
                steps->push_back(s);
            }

            if (isConsistent(r, c, reg, constraints)) {
                fixedMap[r][c] = true;
                if (solveLeaf(idx + 1, tiles, reg, constraints)) return true;
                fixedMap[r][c] = false;
            }

            if (steps && steps->size() < 10000) {
                VisualStep s;
                s.row = r; s.col = c; s.rotation = rot; s.type = "UNDO";
                s.r0 = reg.r0; s.r1 = reg.r1; s.c0 = reg.c0; s.c1 = reg.c1;
                steps->push_back(s);
            }
        }
        board.at(r, c).rotation = orig;
        return false;
    }

    bool solveRegion(const Region &reg, const Constraints &constraints) {
        if (steps && steps->size() < 10000) {
            VisualStep s;
            s.row = -1; s.col = -1; s.type = "REGION";
            s.r0 = reg.r0; s.r1 = reg.r1; s.c0 = reg.c0; s.c1 = reg.c1;
            steps->push_back(s);
        }

        vector<pair<int, int>> varTiles;
        for (int r = reg.r0; r < reg.r1; r++) {
            for (int c = reg.c0; c < reg.c1; c++) {
                if (!fixedMap[r][c]) varTiles.push_back({r, c});
            }
        }

        if (varTiles.empty()) {
            for (int r = reg.r0; r < reg.r1; r++) {
                for (int c = reg.c0; c < reg.c1; c++) {
                    if (!isConsistent(r, c, reg, constraints)) return false;
                }
            }
            return true;
        }

        if (varTiles.size() <= 8) {
            sortTiles_dac(varTiles, board);
            return solveLeaf(0, varTiles, reg, constraints);
        }

        int mid = (reg.width() >= reg.height()) ? (reg.c0 + reg.width() / 2) : (reg.r0 + reg.height() / 2);
        bool vertical = reg.width() >= reg.height();

        sort(varTiles.begin(), varTiles.end(), [&](const pair<int, int> &a, const pair<int, int> &b) {
            bool aInR1 = vertical ? (a.second < mid) : (a.first < mid);
            bool bInR1 = vertical ? (b.second < mid) : (b.first < mid);
            if (aInR1 != bInR1) return aInR1;
            return false; 
        });
        
        return solveLeaf(0, varTiles, reg, constraints);
    }
};

inline bool solve_dac(Board &board, vector<VisualStep>* steps = nullptr) {
    DacSolver solver(board, steps);
    return solver.solve();
}

#endif // DAC_SOLVER_HPP
