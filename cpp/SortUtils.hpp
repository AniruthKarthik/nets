#ifndef SORT_UTILS_HPP
#define SORT_UTILS_HPP

#include "Tile.hpp"
#include <vector>
#include <algorithm>

using namespace std;

// Implementations

inline int getTileDegree_dac(TileType t) {
    switch(t) {
        case CROSS: return 4;
        case T_JUNCTION: return 3;
        case CORNER: return 2;
        case STRAIGHT: return 2;
        case PC: return 1;
        default: return 0;
    }
}

// Heuristic to calculate priority of a tile.
// Higher priority tiles should be solved first.
inline int getTilePriority_dac(const Board &board, int row, int col) {
    int priority = 0;
    const Tile &tile = board.at(row, col);

    // 1. Degree heuristic: Tiles with more connections are more constrained/constraining
    priority += getTileDegree_dac(tile.type) * 10;

    // 2. Fixed/Edge Constraint heuristic
    // Check 4 neighbors
    for (int d = 0; d < 4; d++) {
        Direction dir = static_cast<Direction>(d);
        pair<int, int> nPos = getNeighbor(row, col, dir, board.width, board.height, board.wraps);

        if (nPos.first == -1) {
            // Edge of board (if not wrapping). This is a hard constraint (must be closed).
            priority += 5; 
        } else {
            const Tile &nTile = board.at(nPos.first, nPos.second);
            if (nTile.locked) {
                // Neighbor is locked. Hard constraint.
                priority += 5;
            } else if (nTile.type == EMPTY) {
                // Empty neighbor is effectively locked (no connection).
                priority += 2;
            }
        }
    }

    return priority;
}

// Merge Sort Implementation for Tile Coordinates
// Time Complexity: O(N log N)
// Space Complexity: O(N) auxiliary space
inline void merge_dac(vector<pair<int, int>> &arr, int left, int mid, int right, const Board &board) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temp vectors
    vector<pair<int, int>> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        // Sort descending by priority
        int p1 = getTilePriority_dac(board, L[i].first, L[i].second);
        int p2 = getTilePriority_dac(board, R[j].first, R[j].second);

        if (p1 >= p2) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

inline void mergeSortRecursive_dac(vector<pair<int, int>> &arr, int left, int right, const Board &board) {
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;
    mergeSortRecursive_dac(arr, left, mid, board);
    mergeSortRecursive_dac(arr, mid + 1, right, board);
    merge_dac(arr, left, mid, right, board);
}

// Public interface to sort tiles
inline void sortTiles_dac(vector<pair<int, int>> &tiles, const Board &board) {
    if (tiles.empty()) return;
    mergeSortRecursive_dac(tiles, 0, tiles.size() - 1, board);
}

#endif // SORT_UTILS_HPP
