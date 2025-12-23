# Analysis of Algorithm Complexity - NETS Project

## 1. Overview
This analysis evaluates the Time and Space Complexity of the NETS game project, which consists of a Java-based Controller/UI and a C++ computational engine.

**Variables Used:**
- $N$: Total number of tiles on the board ($N = Rows \times Cols$).
- $V$: Number of vertices in the graph representation (equivalent to $N$).
- $E$: Number of edges in the graph representation (at most $4N$, so $E \in O(N)$).
- $M$: Number of possible moves for a given board state ($M \approx V$).

---

## 2. File-Wise Complexity Breakdown

### File: `cpp/ConnectivityCheck.hpp`
| Function Name | Time Complexity | Space Complexity | Notes |
|--------------|----------------|------------------|-------|
| `dfs` | $O(V)$ | $O(V)$ | Max degree is constant (4), so $V+E \approx V$ |
| `isFullyConnected` | $O(V)$ | $O(V)$ | Single DFS traversal |
| `countComponents` | $O(V)$ | $O(V)$ | Iterates all nodes, DFS on unvisited |
| `countLooseEnds` | $O(V)$ | $O(1)$ | Linear scan of grid |
| `hasClosedLoop` | $O(V)$ | $O(V)$ | Cycle detection via DFS |
| `isSolved` | $O(V \log V)$ | $O(V)$ | Dominated by graph construction |

### File: `cpp/CpuStrategy.hpp`
| Function Name | Time Complexity | Space Complexity | Notes |
|--------------|----------------|------------------|-------|
| `generateMoves` | $O(V)$ | $O(V)$ | Scans grid once |
| `quickSort_partition` | $O(M)$ | $O(1)$ | $M$ is number of moves in range |
| `quickSort_recursive` | $O(M \log M)$ | $O(\log M)$ | Average case sorting |
| `evaluateBoard` | $O(V \log V)$ | $O(V)$ | Builds graph and runs DFS checks |
| `chooseBestMove` | $O(V^2 \log V)$ | $O(V)$ | Evaluates $V$ moves, each taking $O(V \log V)$ |

### File: `cpp/GraphBuilder.hpp`
| Function Name | Time Complexity | Space Complexity | Notes |
|--------------|----------------|------------------|-------|
| `buildGraph` | $O(V \log V)$ | $O(V)$ | `std::map` insertions are $O(\log V)$ |

### File: `com/nets/controller/GameController.java`
| Function Name | Time Complexity | Space Complexity | Notes |
|--------------|----------------|------------------|-------|
| `initGame` | $O(N^2)$ | $O(N)$ | Dominated by `generateGrid` |
| `generateGrid` | $O(N^2)$ | $O(N)$ | Randomized Prim's with `ArrayList` ($O(E)$ remove) |
| `handleHumanMove` | $O(N)$ | $O(N)$ | UI update and simple DFS checks |
| `performStandaloneCpuMove`| $O(N^2)$ | $O(N)$ | Calls C++ engine ($O(N^2 \log N)$ logic) |

---

## 3. Key Algorithmic Operations

### Grid / Graph Construction
- **Time:** $O(V \log V)$
- **Explanation:** The C++ engine converts the 2D grid into an Adjacency List. It uses `std::map` to store nodes, resulting in $O(\log V)$ insertion time per node. Since there are $V$ nodes, the total time is $O(V \log V)$.

### Neighbor Lookup
- **Time:** $O(1)$
- **Explanation:** Handled via direct array indexing checking 4 directions.

### CPU Move Selection (Greedy)
- **Time:** $O(N^2 \log N)$
- **Explanation:**
    1.  Generates all valid moves ($O(N)$).
    2.  Sorts moves using Quick Sort ($O(N \log N)$).
    3.  Iterates through sorted moves. For each move, it builds a graph and evaluates it ($O(N \log N)$).
    4.  Total: $N \times N \log N = O(N^2 \log N)$.

### Sorting (Quick Sort)
- **Time:** $O(M \log M)$
- **Explanation:** A custom Quick Sort implementation prioritizes moves based on tile complexity (Junctions > Corners > Straight).

---

## 4. Overall System Complexity

### Time Complexity
- **Initialization:** $O(N^2)$ due to Prim's Algorithm implementation using `ArrayList`.
- **Human Turn:** $O(N)$ (UI updates + Connectivity Check).
- **CPU Turn:** $O(N^2 \log N)$ (Greedy Strategy evaluation).

### Space Complexity
- **Overall:** $O(N)$
- **Breakdown:** The Grid, Graph Adjacency List, and DFS recursion stacks all scale linearly with the number of tiles.

---

## 5. Observations & Recommendations

### Bottlenecks
1.  **CPU Move Evaluation:** The CPU rebuilds the entire graph ($O(N \log N)$) for *every* candidate move it simulates. This leads to quadratic complexity.
2.  **IPC Overhead:** Transferring the full game state via JSON for every move adds linear overhead $O(N)$ and process creation latency.

### Optimizations
1.  **Incremental Graph Update:** Instead of rebuilding the graph, the CPU could update only the affected edges of the moved tile ($O(\log V)$ or $O(1)$). This would reduce move evaluation to $O(N)$, making the total CPU turn $O(N^2)$ or even $O(N)$ if combined with local connectivity checks.
2.  **HashMap vs TreeMap:** Using `std::unordered_map` in C++ instead of `std::map` would reduce graph construction to $O(V)$ on average.
