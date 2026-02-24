# Backtracking Solver Implementation

This document explains the backtracking solver in `cpp/BtSolver.hpp`.

## Overview

`solve_bt` performs a straightforward **depth-first search** over tile rotations. It assigns rotations in a fixed order and prunes when a rotation conflicts with already-fixed neighbors or the board boundary.

Key ideas:
* Maintain a `fixedMap` of tiles that are already assigned (locked, empty, or chosen during search).
* For each tile, try all legal rotations and verify local consistency.

## Consistency Check

`checkConsistency_bt` checks a single tile against its immediate neighbors:

* If the neighbor is off-board, the tile must not connect in that direction.
* If the neighbor is fixed, the tile’s connection must match the neighbor’s opposite connection.

This check is purely local and is called at each rotation choice.

## Recursive Search

`solveRecursive_bt` performs the backtracking:

1. Select the next tile from `tilesToSolve`.
2. Iterate possible rotations (1, 2, or 4 depending on tile type).
3. If `checkConsistency_bt` passes, mark as fixed and recurse.
4. Backtrack on failure.

## Files and Entry Points

* `cpp/BtSolver.hpp`: Backtracking solver implementation.
* `nets_engine.cpp`: Not currently the default solver; can be wired in by switching `solve_game` to call `solve_bt`.

## Notes

This solver is intentionally simple and is useful as a baseline or correctness reference, but it lacks heuristic ordering or region splitting.
