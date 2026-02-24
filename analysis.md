# Analysis of Algorithm Complexity - NETS Project

## 1. Overview
This analysis evaluates the Time and Space Complexity of the NETS game project, which consists of a Java-based Controller/UI and a C++ computational engine.

**Variables Used:**
- N: Total number of tiles on the board (N = Rows x Cols).
- V: Number of vertices in the graph representation (equivalent to N).
- E: Number of edges in the graph representation (at most 4N, so E is O(N)).
- M: Number of possible moves for a given board state (M approx 3N).

---

## 2. File-Wise Complexity Breakdown

### File: cpp/ConnectivityCheck.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| dfs | O(N) | O(N) | Standard DFS; N nodes, 4N edges max |
| isFullyConnected | O(N) | O(N) | Single DFS traversal |
| countComponents | O(N) | O(N) | Iterates all nodes, DFS on unvisited |
| countLooseEnds | O(N) | O(1) | Linear scan of grid, constant neighbor checks |
| dfsCycleDetection | O(N) | O(N) | DFS carrying parent pointer |
| hasClosedLoop (node) | O(N) | O(N) | Cycle check from specific node |
| hasClosedLoop (graph) | O(N) | O(N) | Checks all components for cycles |
| hasClosedLoop (board) | O(N log N) | O(N) | Dominated by buildGraph |
| isSolved | O(N log N) | O(N) | Dominated by buildGraph |

### File: cpp/CpuStrategy.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| generateMoves_greedy | O(N) | O(N) | Scans grid once, stores valid moves |
| calculateLocalFit_greedy | O(1) | O(1) | Checks 4 neighbors |
| swapMoves | O(1) | O(1) | Simple swap |
| quickSort_partition_greedy | O(M) | O(1) | M is partition size |
| quickSort_recursive_greedy | O(M log M) | O(log M) | Average case sorting |
| sortMoves_greedy | O(M log M) | O(log M) | Calls recursive quick sort |
| evaluateBoard_greedy | O(N log N) | O(N) | Builds graph and runs DFS checks |
| chooseBestMove_greedy | O(N^2 log N) | O(N) | Evaluates O(N) moves, each taking O(N log N) |

### File: cpp/DacSolver.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| checkConsistency_dac | O(1) | O(1) | Checks 4 neighbors for port matching and cut constraints |
| solveRegionEnumerate_dac | O(k^N) | O(N) | k is rotations (max 4). Pruned by consistency checks. |
| solve_dac | O(k^N) | O(N) | Splits the board and combines solutions via cut-edge constraints. |

### File: cpp/BtSolver.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| checkConsistency_bt | O(1) | O(1) | Checks 4 neighbors for port matching |
| solveRecursive_bt | O(k^N) | O(N) | k is rotations (max 4). Pruned by consistency checks. |
| solve_bt | O(k^N) | O(N) | Direct recursive search across all tiles (separate solver) |

### File: cpp/DpSolver.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| portMaskForTile | O(1) | O(1) | Cached port masks for TileType + rotation |
| rotationOptions | O(1) | O(1) | Rotation enumeration for a tile |
| solveFrom | Exponential (memoized) | O(S) | S is number of DP states; memoized recursion over frontier state |
| solve_dp | Exponential (memoized) | O(S) | DP solver with memoization and compact frontier state |

### File: cpp/SortUtils.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| getTilePriority_dac | O(1) | O(1) | Fixed neighbor checks |
| merge_dac | O(N) | O(N) | Merge step of merge sort |
| mergeSortRecursive_dac | O(N log N) | O(N) | Classic Divide & Conquer sorting |
| sortTiles_dac | O(N log N) | O(N) | Sorts tiles for solver optimization |

### File: cpp/GraphBuilder.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| Graph::addEdge | O(log N) | O(1) | Map insertion |
| Graph::nodeCount | O(1) | O(1) | Map size check |
| buildGraph | O(N log N) | O(N) | N iterations, each with map lookups/inserts |

### File: cpp/Tile.hpp
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| getActivePorts | O(1) | O(1) | Fixed number of ports/rotations |
| opposite | O(1) | O(1) | Arithmetic op |
| rotateDirection | O(1) | O(1) | Arithmetic op |
| getNeighbor | O(1) | O(1) | Arithmetic checks |

### File: netgame/src/main/java/com/nets/controller/GameController.java
| Function Name | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| initGame | O(N^2) | O(N) | Calls createNewGameState |
| createNewGameState | O(N^2) | O(N) | Loop until valid grid (usually 1 try) |
| toggleSolution | O(N) | O(1) | Iterates grid to update view |
| generateGrid | O(N^2) | O(N) | Randomized Prims with O(N) removal |
| validateGeneratedGrid | O(N) | O(N) | DFS checks |
| addEdges | O(1) | O(1) | Adds up to 4 edges |
| assignWireType | O(1) | O(1) | Logic checks |
| setupEventHandlers | O(N) | O(1) | Attaches listeners to N tiles |
| handleHumanMove | O(N log N) | O(N) | UI update + Engine Stats (O(N log N)) |
| invokeCppEngine | O(N) | O(N) | JSON Serialization/Deserialization |
| performStandaloneCpuMove | O(N^2 log N) | O(N) | Calls C++ engine |
| updateStats | O(N log N) | O(N) | Calls C++ engine stats |
| calculateLooseEnds | O(N) | O(1) | Linear scan |
| calculateComponents | O(N) | O(N) | DFS based |
| dfs | O(N) | O(N) | Standard traversal |
| updatePoweredStatus | O(N) | O(N) | DFS from power source |
| getConnections | O(1) | O(1) | Switch case check |
| checkWinCondition | O(N) | O(1) | Linear scan for powered status |

---

## 3. Key Algorithmic Operations

### Grid / Graph Construction
- **Time:** O(N log N)
- **Explanation:** The C++ engine converts the 2D grid into an Adjacency List. It uses std::map to store nodes, resulting in O(log N) insertion time per node. Since there are N nodes, the total time is O(N log N).

### Neighbor Lookup
- **Time:** O(1)
- **Explanation:** Handled via direct array indexing checking 4 directions.

### CPU Move Selection (Greedy)
- **Time:** O(N^2 log N)
- **Explanation:**
    1.  Generates all valid moves (O(N)).
    2.  Sorts moves using Quick Sort (O(N log N)).
    3.  Iterates through sorted moves. For each move, it builds a graph and evaluates it (O(N log N)).
    4.  Total: N x N log N = O(N^2 log N).

### Global Solver (DP)
- **Time:** O(k^N) worst case, significantly less in practice.
- **Explanation:**
    1.  Uses **Recursive Enumeration** to search for a valid configuration.
    2.  **Cached Port Masks** (O(1) per step) speed consistency checks.
    3.  **Consistency Checks** prune the search tree early.

### Optional Solver (D&C)
- **Time:** O(k^N) worst case, significantly less in practice.
- **Explanation:**
    1.  Splits the board into sub-regions (Divide and Conquer).
    2.  Uses cut-edge constraints to combine sub-solutions.
    3.  Leaf regions use **Merge Sort (Divide & Conquer)** to order tiles before enumeration.

### Sorting Algorithms
- **Quick Sort:** O(M log M) average case. Used for CPU move prioritization.
- **Merge Sort:** O(N log N) guaranteed. Used for tile prioritization in the solver. This follows the **Divide and Conquer** paradigm.

---

## 4. Overall System Complexity

### Time Complexity
- **Initialization:** O(N^2) due to Prims Algorithm implementation using ArrayList.
- **Human Turn:** O(N log N) (UI updates + Stats from Engine).
- **CPU Turn (Greedy):** O(N^2 log N).
- **Global Solve (DP):** O(k^N) worst case.

### Space Complexity
- **Overall:** O(N)
- **Breakdown:** The Grid, Graph Adjacency List, and recursion stacks (DFS, QuickSort, MergeSort, Recursive Enumeration) all scale linearly with the number of tiles $N$ (or $\log N$ for stacks).

---

## 5. Observations & Recommendations

### Bottlenecks
1.  **CPU Move Evaluation:** The CPU rebuilds the entire graph (O(N log N)) for every candidate move it simulates. This leads to quadratic complexity.
2.  **IPC Overhead:** Transferring the full game state via JSON for every move adds linear overhead O(N) and process creation latency.

### Optimizations
1.  **Incremental Graph Update:** Instead of rebuilding the graph, the CPU could update only the affected edges of the moved tile (O(log N) or O(1)). This would reduce move evaluation to O(N), making the total CPU turn O(N^2) or even O(N) if combined with local connectivity checks.
2.  **HashMap vs TreeMap:** Using std::unordered_map in C++ instead of std::map would reduce graph construction to O(N) on average.
