# Analysis of Algorithm Complexity - NETS Project

## 1. Overview
This document analyzes the Time and Space Complexity of the NETS game project. The system is a hybrid application with a **Java** frontend (Controller/UI) and a **C++** backend (Computational Engine).

**Definitions:**
- **N**: Total number of tiles on the board ($N = Rows \times Cols$).
- **V**: Number of vertices in the graph representation ($V = N$).
- **E**: Number of edges in the graph representation ($E \le 4N$, so $O(E) = O(N)$).
- **M**: Number of possible moves ($M \approx V$, specifically $3V$ as each tile has 3 alternative rotations).

---

## 2. Java Controller Analysis (`netgame/...`)

### Initialization & Grid Generation
The `GameController::generateGrid` method uses a Randomized Prim's Algorithm to generate a perfect maze (spanning tree).

- **Current Complexity:** $O(N^2)$
- **Reasoning:**
    - The algorithm maintains a list of potential edges (`ArrayList<int[]> edges`).
    - In the main loop (which runs $N$ times to visit all cells), it picks a random edge and removes it: `edges.remove(edgeIdx)`.
    - `ArrayList::remove` is $O(K)$ where $K$ is the current number of edges. Since $K$ scales with $N$, this operation is $O(N)$.
    - Total: $N \times O(N) = O(N^2)$.

### Move Handling
- **Human Move:** $O(N \log N)$
    - The UI update is $O(N)$.
    - However, it calls the C++ engine for stats (`get_stats`), which involves graph construction ($O(N \log N)$).
- **IPC Overhead:** $O(N)$
    - Serializing the grid state to JSON and parsing the response scales linearly with the board size.

---

## 3. C++ Engine Analysis (`cpp/...`)

### Graph Construction (`GraphBuilder.hpp`)
The engine rebuilds the graph from the grid state for every analysis request.

- **Current Complexity:** $O(N \log N)$
- **Reasoning:**
    - The function iterates over all $N$ tiles.
    - It uses `std::map<pair<int, int>, vector<...>>` to store the Adjacency List.
    - Insertions and lookups in a `std::map` (Red-Black Tree) take $O(\log V) = O(\log N)$.
    - Total: $N \times O(\log N) = O(N \log N)$.

### Connectivity Checks (`ConnectivityCheck.hpp`)
Algorithms like `countComponents`, `isFullyConnected`, and `hasClosedLoop` utilize Depth First Search (DFS).

- **DFS Complexity:** $O(V + E) = O(N)$
- **Overall Check Complexity:** $O(N \log N)$
    - While the DFS traversal itself is linear $O(N)$, these functions first call `buildGraph`, so they are dominated by the graph construction cost.

### CPU Strategy (`CpuStrategy.hpp`)
The CPU uses a greedy strategy with 1-ply lookahead.

- **Current Complexity:** $O(N^2 \log N)$
- **Reasoning:**
    1.  **Move Generation:** $O(N)$ to find all valid moves.
    2.  **Sorting:** $O(N \log N)$ to sort moves by local fit.
    3.  **Evaluation Loop:** Iterates through up to $M$ moves ($O(N)$).
        - For each candidate move, it copies the board and calls `evaluateBoard`.
        - `evaluateBoard` calls `buildGraph` ($O(N \log N)$) and connectivity checks ($O(N)$).
    - Total: $N \times O(N \log N) = O(N^2 \log N)$.

---

## 4. Summary Table

| Operation | Component | Complexity | Dominating Factor |
|---|---|---|---|
| **Grid Gen** | Java | $O(N^2)$ | `ArrayList::remove` inside loop |
| **Get Stats** | C++ | $O(N \log N)$ | `std::map` insertions in `buildGraph` |
| **CPU Move** | C++ | $O(N^2 \log N)$ | Rebuilding graph for every candidate move |
| **Memory** | Both | $O(N)$ | Grid storage, Adjacency List, Recursion Stack |

---

## 5. Critical Optimizations

### 1. Linear Time Grid Generation ($O(N^2) \to O(N)$)
**Location:** `GameController.java` inside `generateGrid`
- **Change:** Replace `edges.remove(edgeIdx)` with a "Swap-and-Pop" strategy.
- **Details:** Since the order of edges is irrelevant for random selection, swap the selected edge with the last element in the list, then remove the last element. This makes removal $O(1)$.
- **Impact:** Reduces initialization time from Quadratic to Linear.

### 2. Linear Time Graph Construction ($O(N \log N) \to O(N)$)
**Location:** `GraphBuilder.hpp`
- **Change:** Replace `std::map<pair<int,int>, ...>` with `std::vector<vector<int>>`.
- **Details:** Map 2D coordinates $(r, c)$ to a 1D index $idx = r \times Width + c$. Use a vector of size $N$ for the adjacency list.
- **Impact:** Eliminates the $\log N$ tree overhead, making graph building $O(N)$. This would cascade to make `Get Stats` $O(N)$ and `CPU Move` $O(N^2)$.

### 3. Incremental Evaluation
**Location:** `CpuStrategy.hpp`
- **Change:** Avoid rebuilding the entire graph for every simulated move.
- **Details:** When testing a move (rotating one tile), only the edges connected to that specific tile change. Updating the graph locally would take $O(1)$.
- **Impact:** Reduces CPU Move evaluation to $O(N^2)$ (dominated by creating $N$ moves) or even $O(N)$ if we only evaluate the top $K$ candidates.