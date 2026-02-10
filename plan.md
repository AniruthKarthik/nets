# Implementation Plan: Algorithmic Enhancements for Nets

This plan outlines the integration of four key algorithmic concepts—Divide and Conquer, Dynamic Programming, Sorting, and Backtracking—to enhance the Nets game engine (`cpp/` module). The primary goal is to build a robust "Exact Solver" and optimize the engine's decision-making process.

## 1. Backtracking: Exact Puzzle Solver
**Goal:** Implement a deterministic solver that guarantees finding a valid solution for any generated board. The current `CpuStrategy` uses a Greedy approach, which is fast but can get stuck in local optima.

*   **New Component:** `cpp/BacktrackingSolver.hpp`
*   **Algorithm:**
    *   **State Space Search:** Treat the board as a state space where each tile has 4 possible states (rotations).
    *   **Recursive Function:** `solve(index)` processes tiles one by one.
    *   **Constraints:** For the current tile, try only rotations that are compatible with already-placed neighbors (e.g., if the tile above has a "Down" connection, the current tile must have an "Up" connection).
    *   **Backtrack:** If no rotation fits the current constraints, return `false` and backtrack to the previous tile to try a different rotation.

## 2. Sorting: Heuristic Variable Ordering
**Goal:** Optimize the Backtracking solver by intelligently choosing *which* tile to solve next, rather than just going in order (0, 1, 2...).

*   **Integration:** Inside `BacktrackingSolver`.
*   **Algorithm:**
    *   **Most Constrained Variable (MCV):** Prioritize tiles that are harder to fit.
    *   **Sorting Criteria:** Sort tiles based on:
        1.  **Fixed Neighbors:** Tiles adjacent to locked tiles (Power Source) or board edges.
        2.  **Connectivity:** Tiles with more wires (e.g., "Cross" or "T-Junction" types) are more restrictive than "Straight" or "Empty" tiles.
    *   Processing these tiles first prunes the search tree significantly, as invalid configurations are detected earlier.

## 3. Divide and Conquer: Merge Sort Implementation
**Goal:** Implement the sorting mechanism required for the Heuristic Ordering using a custom Divide and Conquer approach.

*   **New Component:** `cpp/SortUtils.hpp`
*   **Algorithm:**
    *   **Merge Sort:** A classic D&C algorithm with $O(n \log n)$ complexity.
    *   **Divide:** Recursively split the array of tiles (or tile indices) into two halves.
    *   **Conquer:** Recursively sort each sub-array (`mergeSortRecursive_dac`).
    *   **Combine:** Merge the two sorted sub-arrays into a single sorted list based on the MCV heuristic defined above (`merge_dac`).
    *   **Usage:** This custom sort function (`sortTiles_dac`) will be called by the `BacktrackingSolver` before starting the recursion.

## 4. Dynamic Programming: Connectivity Memoization
**Goal:** Drastically reduce the computational overhead of validity checks during the backtracking process.

*   **New Component:** `cpp/ConnectivityCache.hpp`
*   **Algorithm:**
    *   **Problem:** In the backtracking loop, the engine repeatedly checks "Does Tile T rotated R connect with Neighbor N?". Doing this via raw logic (checking booleans) millions of times is slow.
    *   **DP Approach (Memoization):** Pre-compute the validity of transitions.
    *   **State Definition:** `dp[TileType][Rotation][NeighborMask]`
        *   `TileType`: The type of the current tile.
        *   `Rotation`: 0-3.
        *   `NeighborMask`: A 4-bit integer representing the required connections from the 4 neighbors (Up, Right, Down, Left).
    *   **Lookup:** The table stores a boolean or a list of valid next states. Instead of calculating logic, the solver performs an $O(1)$ array lookup to see if a move is valid.

## Summary of Execution Flow
1.  **Request:** User asks for a "Hint" or "Auto-Solve".
2.  **Sort (Divide & Conquer):** The engine creates a list of all tiles and sorts them using **Merge Sort** (`sortTiles_dac`) based on complexity constraints.
3.  **Solver (Backtracking):** The engine starts the recursive backtracking process (`solveBacktracking_dac`) on the sorted list.
4.  **Check (Dynamic Programming):** At each step, the solver uses the **DP Cache** to instantly validate if a rotation fits with its neighbors.
