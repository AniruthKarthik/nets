# Analysis of Algorithm Complexity - NETS Project (Optimized)

## 1. Overview
This updated analysis evaluates the Time and Space Complexity of the NETS project following the implementation of unified solver logic and optimized backtracking, dynamic programming, and divide-and-conquer strategies.

**Variables Used:**
- N: Total number of tiles on the board (N = Rows x Cols).
- W: Width of the board.
- k: Maximum number of rotations per tile (k=4).

---

## 2. Solver Complexity Breakdown

### File: cpp/BtSolver.hpp (Backtracking)
| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| Backtracking (solve_bt) | $O(k^N)$ | $O(N)$ | Uses Minimum Remaining Values (MRV) heuristic and early pruning. Highly effective for smaller grids. |

### File: cpp/DpSolver.hpp (Dynamic Programming)
| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| DP Solver (solve_dp) | $O(N \cdot 2^W)$ | $O(N \cdot 2^W)$ | Optimized row-major traversal with memoized frontier state. Most efficient for boards with small width $W$. |

### File: cpp/DacSolver.hpp (Divide and Conquer)
| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| D&C Solver (solve_dac) | $O(k^N)$ | $O(N)$ | Recursive region-splitting with boundary-matching. Reduces effective search space on structured grids. |

---

## 3. Utility Complexity

### File: cpp/SortUtils.hpp (Merge Sort)
| Function | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| sortTiles_dac | $O(N \log N)$ | $O(N)$ | Classic **Divide and Conquer** sorting algorithm used to prioritize tiles by constraint degree. |

### File: cpp/CpuStrategy.hpp (Quick Sort)
| Function | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| sortMoves_greedy | $O(M \log M)$ | $O(\log M)$ | Quick Sort algorithm for CPU move prioritization based on local fit heuristics. |

### File: cpp/ConnectivityCheck.hpp (DFS)
| Function | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| isSolved | $O(N \log N)$ | $O(N)$ | Verifies win condition (connectivity, acyclicity, loose ends) using DFS traversal. |

---

## 4. Key Performance Enhancements

### Unified Port Logic
By centralizing port mask calculation in `SolverUtils.hpp`, all solvers now share a consistent, high-performance logic for rotation evaluation.

### Pruning and Heuristics
The use of **Merge Sort (Divide and Conquer)** to order tiles by their constraint degree (degree + adjacency to boundaries) allows the recursive solvers (BT and DAC) to prune search branches much earlier, avoiding $O(k^N)$ worst-case scenarios in practice.

### Frontier State Memoization
The DP solver effectively manages the state-space search by only storing the necessary boundary information ("frontier"), leading to linear-time complexity with respect to the number of rows for a fixed width.

---

## 5. Summary Table

| Paradigm | Algorithm | Best For | Typical Time |
|---|---|---|---|
| **Greedy** | CPU Strategy | Real-time moves | $O(N^2 \log N)$ |
| **Backtracking** | BT Solver | Small/Medium grids | $O(k^N)$ (Pruned) |
| **DP** | DP Solver | Small width, any height | $O(N \cdot 2^W)$ |
| **Divide & Conquer** | DAC Solver | Large structured grids | $O(k^N)$ (Pruned) |
| **Sorting (D&C)** | Merge Sort | Tile prioritization | $O(N \log N)$ |
| **Sorting** | Quick Sort | Move selection | $O(N \log N)$ |
| **Traversal** | DFS | Win condition checks | $O(N)$ |
