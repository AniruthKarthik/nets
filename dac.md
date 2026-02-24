# Divide and Conquer Implementation: Board-Splitting Solver

This document describes the **Divide and Conquer (D&C)** solver implemented in `cpp/DacSolver.hpp`. The solver splits a board region into two sub-regions, solves one, then constrains the other with boundary-edge requirements.

## Overview

`solve_dac` operates on a rectangular `Region_dac` and recursively splits it until the region is small enough to enumerate directly.

Key ideas:
* **Divide:** Split along the longer axis (`width >= height` splits vertically, otherwise horizontally).
* **Conquer:** Solve the first sub-region.
* **Combine:** Convert the solved sub-region’s cut-edge connections into explicit constraints and solve the second sub-region under those constraints.

For small regions, the solver falls back to a **leaf enumerator** that uses heuristic ordering.

## Data Model

* **`Region_dac`**: Half-open rectangle `[r0, r1) x [c0, c1)`.
* **Edge constraints**: `unordered_map<long long, bool>` keyed by an ordered pair of adjacent cells. The value indicates whether that edge must be connected.

## Main Flow

### 1. Entry Point

`solve_dac` creates the full-board region and calls `solveRegionWithCallback_dac` with empty constraints.

### 2. Region Solver (`solveRegionWithCallback_dac`)

The region solver has three cases:

1. **No variable tiles**: Validate all fixed tiles against current constraints with `validateFixedTiles_dac`. If consistent, invoke the callback.
2. **Leaf region**: If the region has `<= leafThreshold` variable tiles or is 1-row/1-col, it enumerates all rotations using `solveRegionEnumerate_dac`.
3. **Split region**: Split into two halves. Solve the first half, then add cut-edge constraints derived from its boundary tiles and solve the second half.

### 3. Leaf Enumeration

`solveRegionEnumerate_dac` performs a standard recursive rotation search with early pruning:

* **Consistency checking** uses `checkConsistency_dac`, which verifies:
  * Board edges are respected (no open connections off-board).
  * Already-fixed neighbors inside the region match.
  * Fixed neighbors outside the region match.
  * Explicit edge constraints are satisfied.

* **Tile ordering** is done by `sortTiles_dac` (in `cpp/SortUtils.hpp`) using an MCV-style heuristic to reduce branching.

## Cut-Edge Constraints

When a region is solved, the solver inspects the tiles along the split boundary:

* **Vertical split**: For each row `r`, it examines `(r, mid-1)` and records whether that tile connects **east** across the cut.
* **Horizontal split**: For each column `c`, it examines `(mid-1, c)` and records whether that tile connects **south** across the cut.

These constraints are added to the map passed to the second region. If a constraint already exists and conflicts, the solve path fails early.

## Files and Entry Points

* `cpp/DacSolver.hpp`: Main D&C solver.
* `cpp/SortUtils.hpp`: Tile ordering helper used by the leaf enumerator.
* `nets_engine.cpp`: The engine routes `solve_game` to a specific solver (currently `solve_dp`).

## Performance Notes

Splitting reduces the effective search space and exposes early contradictions across the cut. On small regions, heuristic ordering still provides most of the pruning benefit.
