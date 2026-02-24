# DP Solver Implementation (Rotation Search with Bitmasks)

This document explains the solver in `cpp/DpSolver.hpp`. Despite the name, it is a **depth-first rotation search** with pruning; it does not use memoized subproblem reuse or table-based dynamic programming.

## Overview

`solve_dp` builds a list of variable tiles (not locked, not empty) and recursively assigns rotations. It prunes early using a fast **bitmask-based consistency check**.

Key ideas:
* Precompute a **4-bit port mask** for each tile rotation.
* Maintain a `fixedMap` indicating which tiles are already assigned.
* At each step, try the allowed rotations for a tile and validate adjacency against fixed neighbors and board edges.

## Port Mask Computation

`portMaskFromTile_dp` returns a 4-bit mask (N/E/S/W). It has two modes:

* **Custom tiles**: Compute the mask directly from the tile’s custom connections.
* **Standard tiles**: Use a lazily initialized lookup table `masks[7][4]` so rotation masks are constant-time.

## Consistency Check

`checkConsistency_dp` computes two masks for the current tile:

* `mustConnect`: edges that must be connected (neighbor fixed with a matching port).
* `mustNotConnect`: edges that must be empty (off-board or neighbor fixed without a matching port).

A rotation is valid only if:

* It does not connect through any `mustNotConnect` edge.
* It includes all `mustConnect` edges.

This is a fast bitwise check.

## Recursive Search

`solveRecursive_dp` does the following:

1. Pick the next tile in `tilesToSolve`.
2. Iterate all valid rotations (1, 2, or 4 depending on tile type).
3. If `checkConsistency_dp` passes, mark the tile fixed and recurse.
4. Backtrack if a rotation fails downstream.

## Files and Entry Points

* `cpp/DpSolver.hpp`: DP-labeled solver implementation.
* `nets_engine.cpp`: The engine currently routes `solve_game` to `solve_dp`.

## Notes

This solver is **not divide-and-conquer**. It is a single-region recursive search with bitmask pruning. If you want a true D&C approach, use `solve_dac` in `cpp/DacSolver.hpp`.
