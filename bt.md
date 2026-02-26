# Backtracking Solver Implementation

This document describes the optimized backtracking solver in `cpp/BtSolver.hpp`.

## Overview

`solve_bt` uses recursive depth-first search to find a valid tile configuration. It is enhanced with heuristic tile ordering and early pruning to improve performance.

### Key Components:
- **`BtSolver` Class:** Manages the search state and recursion.
- **`SolverUtils.hpp`:** Provides unified port mask calculation and consistency checks.
- **Tile Prioritization:** Tiles are pre-sorted using a heuristic (Minimum Remaining Values approach) to prioritize more constrained tiles (edges, high-degree tiles).

## Consistency Checking
At each step, the solver checks if the current rotation of a tile is compatible with:
1.  **Board Boundaries:** No ports can point off-board (unless wrapping).
2.  **Fixed Neighbors:** Connections must match the ports of adjacent tiles that are already fixed (locked or previously assigned).

## Complexity
- **Time Complexity:** $O(k^N)$ in the worst case, where $N$ is the number of tiles and $k$ is the max rotations (4). However, heuristic ordering and pruning significantly reduce the explored state space.
- **Space Complexity:** $O(N)$ for the recursion stack and the fixed-tile map.

## Usage
The engine can be switched to use `solve_bt` by modifying the `solve_game` action in `nets_engine.cpp`.
