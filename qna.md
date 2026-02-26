# Design and Analysis of Algorithms - Project Q&A

## 1. Algorithmic Paradigms Used

### What are the main algorithmic paradigms used in this project?
1.  **Greedy Algorithms:** Used by the CPU to pick the immediate best move based on local heuristics.
2.  **Dynamic Programming (DP):** Used in the `solve_dp` solver to store and reuse subproblem solutions via memoized recursion over a frontier state.
3.  **Backtracking (BT):** Used in `solve_bt` and `solve_dac` to systematically explore the state space of tile rotations.
4.  **Divide and Conquer (D&C):**
    - **Merge Sort:** Implemented in `SortUtils.hpp` to sort tiles by their constraint priority.
    - **DAC Solver:** Implemented in `DacSolver.hpp` to recursively split the board into regions and solve them based on boundary compatibility.
5.  **Graph Traversal (DFS):** Used for connectivity checks, component counting, and cycle detection.

---

## 2. Complexity Analysis

### What is the time complexity of the DP solver?
The DP solver operates in $O(N \cdot 2^W)$ time, where $N$ is the total number of tiles and $W$ is the width of the board. This is because the state is defined by the tile index and the "frontier" of connections (up to $W$ bits).

### Why use Merge Sort for tile prioritization in the solvers?
Merge Sort is a stable, **Divide and Conquer** algorithm that guarantees $O(N \log N)$ performance. We use it to sort tiles based on their "constraint degree" (number of fixed neighbors, board edges, and connections). By solving the most constrained tiles first, we prune the backtracking search tree as early as possible.

### What is the complexity of the CPU move selection?
The CPU evaluates all possible rotations for every unlocked tile ($O(N)$ moves). For each move, it performs a local fit calculation ($O(1)$) and uses **Quick Sort** ($O(N \log N)$) to prioritize moves. The total selection time is $O(N^2 \log N)$ if it simulates each top move.

---

## 3. Data Structures

### How is the network modeled?
The game is modeled as a **Graph** where each tile is a node and each connection is an edge. We use an **Adjacency List** (`map<pair<int, int>, vector<pair<int, int>>>`) for space efficiency $O(V+E)$ and quick neighbor traversal.

---

## 4. Problem Solving Strategies

### How do we handle "wraps" in the solvers?
For wrapping boards, the solvers (DP especially) must account for the connection between the last row/column and the first row/column. The DP solver brute-forces the initial boundary constraints and ensures they match at the end of the recursion.

### How is the win condition verified?
We use DFS from the power source to ensure:
1.  All PCs are reachable (connectivity).
2.  The number of visited nodes equals the total non-empty nodes.
3.  No back-edges are found during DFS (acyclicity).
4.  No "loose ends" exist (all ports have a matching neighbor).

---

## 5. Implementation Details

### Why a hybrid Java/C++ architecture?
- **JavaFX** provides a modern, responsive UI.
- **C++** offers high performance for computationally intensive tasks like recursive state-space exploration (BT, DP, DAC) and graph analysis.
- **JSON/IPC** serves as a clean interface between the two components.
