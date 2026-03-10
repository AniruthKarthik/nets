# Analysis of Algorithm Complexity - NETS Project (Optimized)

## 1. Overview
This updated analysis evaluates the Time and Space Complexity of the NETS project following the implementation of unified solver logic and optimized backtracking, dynamic programming, and divide-and-conquer strategies.

**Variables Used:**
- $N$: Total number of tiles on the board ($N = \text{Rows} \times \text{Cols}$).
- $W$: Width of the board.
- $k$: Maximum number of rotations per tile ($k=4$).

---

## 2. Solver Complexity Breakdown

### File: cpp/BtSolver.hpp (Backtracking)
| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| Backtracking (solve_bt) | $O(k^N)$ | $O(N)$ | Uses Minimum Remaining Values (MRV) heuristic and early pruning. Highly effective for smaller grids. |

### File: cpp/DpSolver.hpp (Dynamic Programming)
| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| DP Solver (solve_dp) | $O(N \cdot 2^W)$ | $O(N \cdot 2^W)$ | Optimized row-major traversal with memoized frontier state. Most efficient for boards with small width $W$. For wrapping boards, time is $O(N \cdot 4^W)$. |

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
| isSolved | $O(N)$ | $O(N)$ | Verifies win condition (connectivity, acyclicity, loose ends) using optimized DFS traversal with integer-indexed adjacency lists and $O(1)$ visited checks. |

---

## 4. Key Performance Enhancements

### Low-Level Bitwise Optimization
Port matching and constraint evaluation have been moved from `vector`-based list comparisons to `uint8_t` bitmask operations. This allows the solvers to perform rotation compatibility checks in constant time ($O(1)$) with zero memory allocations in the core search loop.

### 1D Grid Flattening
The board representation was migrated from a 2D `vector<vector<Tile>>` to a flattened 1D `vector<Tile>`. This optimization:
1.  **Improves Cache Locality:** Tiles are contiguous in memory, reducing CPU cache misses during traversal.
2.  **Reduces Overhead:** Minimizes memory management overhead when copying the board for move evaluation.

### Integer-Indexed Adjacency Lists
The graph structure now uses a flattened `std::vector` for adjacency storage instead of a `std::map`. This removes the $O(\log V)$ lookup cost, enabling $O(1)$ access to neighbor lists.

### Unified Port Logic
By centralizing port mask calculation in `SolverUtils.hpp`, all solvers now share a consistent, high-performance logic for rotation evaluation.

### Pruning and Heuristics
The use of **Merge Sort (Divide and Conquer)** to order tiles by their constraint degree (degree + adjacency to boundaries) allows the recursive solvers (BT and DAC) to prune search branches much earlier, avoiding $O(k^N)$ worst-case scenarios in practice.

### Frontier State Memoization
The DP solver effectively manages the state-space search by only storing the necessary boundary information ("frontier"), leading to linear-time complexity with respect to the number of rows for a fixed width. For wrapping boards, it iterates through all boundary configurations, resulting in $O(N \cdot 4^W)$ time.

---

## 5. Summary Table

| Paradigm | Algorithm | Best For | Typical Time | Typical Space |
|---|---|---|---|---|
| **Greedy** | CPU Strategy | Real-time moves | $O(N^2)$ | $O(N)$ |
| **Backtracking** | BT Solver | Small/Medium grids | $O(k^N)$ (Pruned) | $O(N)$ |
| **DP** | DP Solver | Small width ($W \le 16$) | $O(N \cdot 2^W)$ | $O(N \cdot 2^W)$ |
| **DP (Wrapping)** | DP Solver | Toroidal grids | $O(N \cdot 4^W)$ | $O(N \cdot 2^W)$ |
| **Divide & Conquer** | DAC Solver | Large structured grids | $O(k^N)$ (Pruned) | $O(N)$ |
| **Sorting (D&C)** | Merge Sort | Tile prioritization | $O(N \log N)$ | $O(N)$ |
| **Sorting** | Quick Sort | Move selection | $O(N \log N)$ | $O(\log M)$ |
| **Traversal** | DFS | Win condition checks | $O(N)$ | $O(N)$ |
