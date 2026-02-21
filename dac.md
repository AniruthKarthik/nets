# Divide and Conquer Implementation: Board-Splitting Solver

This document explains the **Divide and Conquer** (D&C) strategy implemented in the Nets game engine to solve the puzzle by recursively splitting the board into sub-boards.

## Overview

The solver now uses **Divide and Conquer** on the board itself. It splits the board into two regions, solves one region, and then constrains the other region based on the connections across the cut. This drastically reduces the search space compared to a single monolithic exhaustive search.

For small regions (leaf cases), the solver still uses **recursive enumeration** with **Heuristic Variable Ordering** (Merge Sort) to solve quickly.

## Implementation Details

### 1. Divide and Conquer: Board Splitting (`cpp/DacSolver.hpp`)

The entry point is `solve_dac`. It recursively divides the board into sub-regions:

*   **Divide:** Split the region across the longer dimension.
*   **Conquer:** Solve one side, then solve the other side.
*   **Combine:** Enforce consistency across the cut by tracking required connections on the cut edges.

**Boundary Constraints**
When a region is solved, the solver records the connection state of each cut-edge (on/off). These states become constraints for the neighboring region so both halves agree.

### 2. Leaf Solver: Recursive Enumeration + MCV Sort (`cpp/SortUtils.hpp`)

When a region is small enough, the solver switches to a recursive enumeration leaf solver:

*   **Identify Variables:** Collect all non-locked, non-empty tiles in the region.
*   **Sort:** Use Merge Sort + MCV heuristic (`getTilePriority_dac`) to prioritize the most constrained tiles.
*   **Solve:** Standard recursive enumeration with consistency checks.

**MCV Priority Formula:**
```cpp
Priority = (Degree * 10) + (IsEdge ? 5 : 0) + (LockedNeighbor ? 5 : 0)
```
Higher priority tiles are solved first.

## Code Structure

*   **`cpp/DacSolver.hpp`**: Contains `solve_dac`, region splitting, boundary constraints, and leaf enumeration.
*   **`cpp/SortUtils.hpp`**: Contains `mergeSortRecursive_dac`, `merge_dac`, and `getTilePriority_dac` used by the leaf solver.
*   **`nets_engine.cpp`**: Exposes the `solve_game` action (now routed to `solve_dac`).

## Performance Impact

By splitting the board, the solver explores smaller subspaces and prunes early when cut-edge constraints cannot be satisfied. For small regions, the MCV ordering still helps the leaf enumerator fail fast or converge quickly.
