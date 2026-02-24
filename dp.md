# DP Solver Implementation (Memoized Dynamic Programming)

This document explains the solver in `cpp/DpSolver.hpp`. It is a **true DP** solver: it uses an explicit state definition and memoization to reuse subproblem results.

## Overview

`solve_dp` uses row-major traversal (top-left to bottom-right) and a memoized recursion. At each tile it tries valid rotations and uses a compact state to avoid re-solving the same suffix of the board.

Key ideas:
* Precompute a **4-bit port mask** for each tile rotation.
* Maintain a **frontier state** that captures the required north/west connections for the current tile.
* At each step, try the allowed rotations and enforce compatibility with those requirements.
* Memoize the result for each state so equivalent subproblems are solved once.

## Port Mask Computation

`portMaskForTile` returns a 4-bit mask (N/E/S/W). It has two modes:

* **Custom tiles**: Compute the mask directly from the tile’s custom connections.
* **Standard tiles**: Use a lazily initialized lookup table `masks[7][4]` so rotation masks are constant-time.

## State Definition

The state is defined by:

* `idx`: linear index of the current tile (row-major).
* `upMask`: a width-bit mask where bit `c` is the expected **north** connection for the current tile in column `c`.
* `leftReq`: a 1-bit expected **west** connection for the current tile.
* `rowWrapExpected`: for wrap boards, the expected west connection for column 0 (enforced against the last column).

This state uniquely identifies the remaining subproblem without copying the board.

## Memoized Recurrence

`solveFrom` does the following:

1. Derive `r, c` from `idx` and read the expected north/west requirements.
2. Iterate valid rotations for the current tile.
3. Check if the rotation matches expected north/west, and respects edge rules.
4. Update the frontier (`upMask` and `leftReq`) for the next tile.
5. Memoize and reuse the result for identical states.

## Files and Entry Points

* `cpp/DpSolver.hpp`: DP-labeled solver implementation.
* `nets_engine.cpp`: The engine currently routes `solve_game` to `solve_dp`.

## Notes

This solver is **not divide-and-conquer**. It is a row-major DP with memoization and a compact frontier state. If you want D&C, use `solve_dac` in `cpp/DacSolver.hpp`.
