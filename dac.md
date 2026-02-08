# Divide and Conquer Implementation: Heuristic Variable Ordering

This document explains the **Divide and Conquer** (D&C) strategy implemented in the Nets game engine to optimize the exact puzzle solver.

## Overview

The "Exact Solver" uses a **Backtracking** algorithm to find a valid configuration for the game board. A naive backtracking approach (solving tiles from top-left to bottom-right) is inefficient because it often explores deep branches of invalid configurations before failing.

To optimize this, we implement **Heuristic Variable Ordering** using a Divide and Conquer sorting algorithm. By processing the "most constrained" tiles first, we prune the search tree significantly, allowing the solver to fail fast or find solutions quicker.

## Implementation Details

### 1. Divide and Conquer: Merge Sort (`cpp/SortUtils.hpp`)

We implemented a custom **Merge Sort** algorithm to sort the list of tiles (variables) before the solver begins.

*   **Algorithm:** Merge Sort
*   **Time Complexity:** $O(N \log N)$ where $N$ is the number of tiles.
*   **Space Complexity:** $O(N)$ auxiliary space.

**Why Merge Sort?**
Merge Sort is a classic Divide and Conquer algorithm. It recursively breaks the list of tiles into halves ("Divide"), sorts them ("Conquer"), and merges them back together ("Combine"). This structure is efficient and guarantees $O(N \log N)$ performance, unlike QuickSort which can degrade to $O(N^2)$ in worst-case scenarios.

### 2. The Heuristic: Most Constrained Variable (MCV)

The sorting logic (`getTilePriority` function) orders tiles based on how "difficult" they are to place.

*   **Degree Heuristic:** Tiles with more wires (e.g., `CROSS`, `T_JUNCTION`) have fewer valid orientations than simple tiles (e.g., `STRAIGHT`). We solve high-degree tiles first.
*   **Constraint Heuristic:**
    *   **Locked Neighbors:** If a tile is next to a locked tile, it must match that neighbor perfectly. This is a hard constraint.
    *   **Board Edges:** Tiles on the edge have fewer valid rotations because they cannot point wires off the board.

**Priority Formula:**
```cpp
Priority = (Degree * 10) + (IsEdge ? 5 : 0) + (LockedNeighbor ? 5 : 0)
```
Higher priority tiles are placed at the beginning of the list.

### 3. Integration with Backtracking (`cpp/BacktrackingSolver.hpp`)

The `solveBacktracking` function orchestrates the process:

1.  **Identify Variables:** It collects all non-locked, non-empty tiles into a list.
2.  **Sort (Divide & Conquer):** It calls `sortTiles` (our Merge Sort implementation) to reorder this list based on the MCV heuristic.
3.  **Solve (Recursion):** It iterates through the *sorted* list.
    *   For the current tile, it tries all valid rotations.
    *   `checkConsistency` verifies if the rotation fits with currently "fixed" neighbors (neighbors that are either locked or already visited/set by the solver).
    *   If valid, it recurses to the next tile in the sorted list.
    *   If no rotation works, it backtracks.

## Code Structure

*   **`cpp/SortUtils.hpp`**: Contains `mergeSortRecursive`, `merge`, and `getTilePriority`. This is the core D&C implementation.
*   **`cpp/BacktrackingSolver.hpp`**: Contains the `solveBacktracking` entry point and `solveRecursive` logic. It uses `SortUtils` to prepare the data.
*   **`nets_engine.cpp`**: Exposes the `solve_game` action to the external interface.

## Performance Impact

By solving the most constrained parts of the puzzle first (e.g., corners, crowded areas, and high-degree tiles), the backtracking algorithm makes critical decisions early. If a configuration is impossible, it is detected near the root of the recursion tree rather than deep in the leaves, resulting in a dramatic reduction in the number of visited states.
