# Divide and Conquer Solver Implementation

This document describes the Board-Splitting (D&C) solver in `cpp/DacSolver.hpp`.

## Overview

`solve_dac` is a recursive solver that structures the search space by splitting the board into regions. It prioritizes solving one sub-region before moving to the next, effectively acting as a Divide and Conquer strategy for state-space search.

### Algorithmic Approach:
1.  **Divide:** The board is recursively split into sub-regions (alternating vertical and horizontal splits).
2.  **Order:** Tiles are ordered such that tiles in the first sub-region are processed before tiles in the second sub-region.
3.  **Conquer:** The solver performs a backtracking search on the ordered tiles.
4.  **Combine:** Consistency checks across the region boundaries ensure that sub-solutions are compatible.

## Region-Based Search
By splitting the board, the solver can detect contradictions early at the boundaries. If a partial solution in the first region cannot be extended into the second region due to boundary constraints, the solver backtracks immediately.

## Performance
- **Heuristic Sorting:** Like the BT solver, DAC uses `sortTiles_dac` to prioritize constrained tiles within each region.
- **Leaf Threshold:** Small regions are solved directly using the optimized leaf enumerator.

## Complexity
- **Time Complexity:** $O(k^N)$ worst-case, but region-based ordering often finds solutions faster on large, structured grids.
- **Space Complexity:** $O(N)$ for the recursion and region metadata.
