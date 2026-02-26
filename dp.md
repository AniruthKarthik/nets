# DP Solver Implementation (Dynamic Programming)

This document describes the memoized row-major solver in `cpp/DpSolver.hpp`.

## Overview

`solve_dp` is a high-performance solver that uses dynamic programming (memoized recursion) over a frontier state. It is the default solver for the NETS engine.

### Key Concepts:
- **Row-Major Traversal:** Tiles are processed from top-left to bottom-right.
- **Frontier State:** The state of the DP is defined by the current tile index and a bitmask of required connections from the row above ("upMask") and the tile to the left ("leftReq").
- **Memoization:** Results for each state `(idx, upMask, leftReq)` are cached in a hash map to avoid redundant calculations.

## State Encoding
The solver uses a 64-bit key to represent the state:
- `idx`: 16 bits
- `upMask`: 16 bits (supports board widths up to 16)
- `leftReq`: 1 bit
This compact representation ensures high hit rates in the memoization table.

## Wrap-Around Support
For wrapping boards, the solver iterates through all possible boundary configurations (initial North and West requirements) and solves for each. This ensures that the global solution respects the toroidal topology of the grid.

## Complexity
- **Time Complexity:** $O(N \cdot 2^W)$ where $W$ is the width of the board. This is significantly better than $O(4^N)$ for boards where the width is relatively small.
- **Space Complexity:** $O(S)$ where $S$ is the number of visited states in the memoization table.
