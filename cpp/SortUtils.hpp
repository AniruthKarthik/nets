#ifndef SORT_UTILS_HPP
#define SORT_UTILS_HPP

#include "Tile.hpp"
#include "SolverUtils.hpp"
#include <vector>
#include <algorithm>

using namespace std;

// Heuristic to evaluate how constrained a tile is.
// High score = more constrained (should be solved earlier).
inline int getConstraintDegree(int r, int c, const Board &b) {
    int degree = 0;
    // Base degree from ports
    uint8_t mask = getPortMask(b.at(r, c));
    for (int d = 0; d < 4; d++) if ((mask >> d) & 1) degree++;

    // Proximity to boundaries
    if (r == 0 || r == b.height - 1) degree += 2;
    if (c == 0 || c == b.width - 1) degree += 2;

    // Neighbors
    for (int d = 0; d < 4; d++) {
        auto n = getNeighbor(r, c, static_cast<Direction>(d), b.width, b.height, b.wraps);
        if (n.first != -1) {
            if (b.at(n.first, n.second).locked) degree += 3;
            if (b.at(n.first, n.second).type == EMPTY) degree += 1;
        } else {
            degree += 2; // Hard boundary
        }
    }
    return degree;
}

// Time Complexity: $O(N \log N)$
// Space Complexity: $O(N)$ auxiliary space
inline void sortTiles_dac(vector<pair<int, int>> &tiles, const Board &b) {
    if (tiles.size() <= 1) return;

    // We use std::stable_sort which is typically a merge sort variant (O(N log N))
    stable_sort(tiles.begin(), tiles.end(), [&](const pair<int, int> &a, const pair<int, int> &b_tile) {
        return getConstraintDegree(a.first, a.second, b) > getConstraintDegree(b_tile.first, b_tile.second, b);
    });
}

#endif // SORT_UTILS_HPP
