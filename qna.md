# Design and Analysis of Algorithms - Project Report

## NETS: Network Connectivity Puzzle

### 1. PROJECT OVERVIEW & MOTIVATION

**What is this project about?**
This project is a digital implementation of the puzzle game "NETS" (often known as "Net Walk" or "Network"). It is a hybrid application featuring a JavaFX-based Graphical User Interface (GUI) and a high-performance C++ computational engine. The objective is to rotate tiles on a grid to connect all computers (clients) to a central power server without creating closed loops or leaving loose ends.

**Why was the NETS game chosen for this project?**
NETS was selected because it naturally models **Graph Theory** problems. The game grid represents a graph where:
*   **Tiles** are Nodes (Vertices).
*   **Wire connections** are Edges.
The core gameplay involves solving connectivity problems (Spanning Trees) and detecting cycles, which are fundamental topics in Design and Analysis of Algorithms (DAA).

**What core problem does this project attempt to solve?**
The project solves the problem of **constructing a valid Spanning Tree** (or a connected subgraph without cycles) from a scrambled grid state using local operations (tile rotations). It effectively simulates a network optimization problem where the goal is to achieve full connectivity with minimum cost (moves).

**What algorithmic concepts does this project emphasize?**
*   **Graph Traversal:** Depth First Search (DFS) for connectivity checks and cycle detection.
*   **Greedy Algorithms:** Used by the CPU opponent to determine the optimal local move.
*   **Minimum Spanning Tree (MST):** Randomized Prim's Algorithm is used for procedural level generation.
*   **Sorting:** Quick Sort is used for moves, and Merge Sort (Divide & Conquer) is used for tile prioritization.
*   **Recursive Enumeration:** A recursive solver with state restoration to find the global solution.
*   **BT (bt) and DP (dp):** Optional variants that perform recursive search, with `dp` using cached port masks. They are separate implementations.
*   **Heuristics:** Scoring functions to evaluate game states and prioritize tile placement.

**How is this project relevant to Design and Analysis of Algorithms (DAA)?**
It demonstrates the practical application of theoretical DAA concepts like **time complexity analysis**, **graph data structures** (Adjacency Lists), **recursion** (DFS, Merge Sort, Recursive Enumeration), and **optimization strategies** (Greedy and D&C approach) in a real-world software system.

**Project Summary:**
A hybrid C++/Java puzzle game that utilizes graph algorithms, greedy strategies, and recursive enumeration with divide-and-conquer sorting to solve network connectivity problems in real-time.

---

### 2. GAME & APPROACH

**Why is a game-based approach suitable here?**
A game provides an interactive visual representation of abstract graph algorithms. It allows users to "see" connectivity and cycles, making complex algorithmic states (like "Disjoint Sets" or "Cyclic Graphs") intuitively understandable.

**Is the game turn-based? Why or why not?**
**Yes.** The game implements a turn-based system (Human vs. CPU). This allows the C++ engine to perform computationally intensive calculations (analyzing potential board states or running the D&C solver) between user moves without UI freezing.

**How does scoring work in this version of NETS?**
While there isn't a traditional "score," the game state is evaluated using a **cost function** (implemented in `CpuStrategy.hpp`):
`Score = (LooseEnds * 10) + (Components * 5) + (HasLoop ? 1000 : 0)`
The goal is to minimize this score. A score of 0 implies a solved state (ideally).

**What happens when a risky / losing move is made?**
If a move creates a **closed loop** (cycle), the heuristic function assigns a massive penalty (`1000` points). This discourages the CPU from making moves that violate the spanning tree structure.

**How is a winner or game-ending condition determined?**
The game ends with a "Victory" when:
1.  **Zero Loose Ends:** No wires pointing to empty space or mismatched connections.
2.  **Single Component:** All tiles are connected in one graph (verified via DFS).
3.  **No Loops:** The graph is acyclic.
4.  **All Powered:** Connectivity traces back to the central `POWER` tile.

**What limitations exist in the current game logic?**
The CPU uses a **1-step lookahead Greedy Strategy**. It evaluates the immediate best move but does not plan multiple turns ahead (like Minimax). However, the system includes a **Global DP Solver** that can be triggered to find a guaranteed solution. Optional solvers (`solve_bt`, `solve_dac`) also exist for comparison and are not combined.

---

### 3. ALGORITHMS USED

**Which algorithms are implemented?**
1.  **Depth First Search (DFS):** For traversing the graph to count connected components and detect cycles.
2.  **Randomized Prim's Algorithm:** Used in `GameController.java` to generate a perfect maze (Spanning Tree) for the level.
3.  **Greedy Best-First Search:** Used by the CPU to pick the optimal move.
4.  **DP Solver:** A memoized dynamic-programming solver that explores the state space with a compact frontier state.
5.  **D&C Solver:** A recursive algorithm that splits the board and combines sub-solutions via cut-edge constraints.
6.  **Quick Sort:** Used to sort potential CPU moves.
7.  **Merge Sort (Divide & Conquer):** Used to prioritize tiles for the D&C solver.
8.  **BT Solver:** A direct recursive search used for comparison.

**Where is graph traversal used?**
*   **Cycle Detection:** `hasClosedLoop` in `ConnectivityCheck.hpp` uses DFS to find back-edges.
*   **Component Counting:** `countComponents` uses iterative DFS to find the number of disjoint subgraphs.
*   **Power Propagation:** Java's `updatePoweredStatus` uses DFS to light up connected cables from the server.

**Where is a greedy strategy applied?**
In `CpuStrategy.hpp`, the function `chooseBestMove_greedy`:
1.  Generates all possible rotations for every unlocked tile.
2.  Sorts them by "local fit" (matching neighbors) using Quick Sort.
3.  Simulates the top moves on a temporary board.
4.  **Greedily** selects the move with the lowest heuristic cost.

**How does the DP Solver work?**
Implemented in `DpSolver.hpp`, it uses memoized recursion with a compact frontier state:
1.  It defines a **State** using the tile index, a per-column north-connection mask, and the expected west connection.
2.  It uses **Cached Port Masks** to test rotations quickly.
3.  It memoizes each state to avoid recomputation and reuses results across equivalent subproblems.

**How does the D&C Solver work?**
Implemented in `DacSolver.hpp`, it recursively splits the board into sub-regions and combines them using cut-edge constraints. Leaf regions use Merge Sort (MCV) ordering before enumeration.

**What are the solver variants?**
`solve_bt` performs direct recursive search with neighbor consistency checks. `solve_dac` performs board splitting with cut-edge constraints. They are separate solvers.

**Is sorting used? If yes, where and why?**
**Yes, two different sorting algorithms are used:**
1.  **Quick Sort:** In `CpuStrategy.hpp` to prioritize CPU moves by their immediate local connection quality.
2.  **Merge Sort:** In `SortUtils.hpp` (`sortTiles_dac`) to sort tiles by their constraint priority (degree + edge location). This is a classic **Divide and Conquer** implementation.

**Why was a greedy approach selected for the CPU?**
A Greedy approach is computationally efficient ($O(N)$ moves to evaluate) compared to exhaustive solvers ($O(4^N)$), making it suitable for a responsive real-time game.

**How is risk or move evaluation calculated?**
Risk is calculated via the linear heuristic:
*   **Loops:** Extremely High Risk (Penalty 1000).
*   **Loose Ends:** Medium Risk (Penalty 10).
*   **Disconnection:** Low Risk (Penalty 5 per component).

---

### 4. GRAPH & DATA STRUCTURES

**Is the game modeled as a graph? Why?**
**Yes.** The grid structure is deceptive; the underlying logic is pure graph theory. Connectivity and cycles are properties of graphs, not just 2D arrays.

**How is the graph represented internally?**
In `GraphBuilder.hpp`, the graph is built as an **Adjacency List**:
`map<pair<int, int>, vector<pair<int, int>>> adjList;`
*   **Keys:** Node coordinates `(row, col)`.
*   **Values:** List of connected neighbors.

**Why was an adjacency list preferred?**
An adjacency list is space-efficient $O(V+E)$ for sparse graphs like this grid (where max degree is 4). An Adjacency Matrix would be $O(V^2)$, which is wasteful for a grid.

**Why were HashMap (Map), ArrayList (Vector), or arrays used?**
*   **`std::map`**: Used in C++ to map coordinates to neighbor lists.
*   **`std::vector`**: Dynamic arrays used for lists of moves and neighbors, offering $O(1)$ access.
*   **Java Arrays (`Tile[][]`)**: Used for the visual grid representation.

**What is the maximum degree of a node?**
**4**. A tile can connect to North, East, South, and West.

---

### 5. CPU / AI LOGIC

**How does the CPU decide its move?**
1.  **Generate:** List all legal rotations.
2.  **Sort (Quick Sort):** Prioritize moves that look locally correct.
3.  **Simulate:** Apply move -> Build Graph -> Evaluate Metric.
4.  **Select:** Pick the move resulting in the lowest metric.

---

### 6. SORTING

**Why was Quick Sort used for CPU moves?**
It is generally efficient with $O(N \log N)$ performance and sorts the moves in-place, which is useful when generating thousands of potential states.

**Why was Merge Sort used for the Solver?**
Merge Sort is a stable, **Divide and Conquer** algorithm. It was chosen to sort tiles by priority because it guarantees $O(N \log N)$ performance regardless of the input distribution, ensuring the solver starts with the most constrained tiles first.

**Time complexity of the sorting algorithms:**
*   **Quick Sort:** Average $O(N \log N)$, Worst $O(N^2)$.
*   **Merge Sort:** Always $O(N \log N)$.

**Space complexity of the sorting algorithms:**
*   **Quick Sort:** $O(\log N)$ stack space.
*   **Merge Sort:** $O(N)$ auxiliary space for merging.

---

### 7. TIME & SPACE COMPLEXITY

**Time Complexity:**
*   **Initialization (Prim's):** $O(N^2)$ (due to randomized selection in Java).
*   **Graph Build:** $O(N \log N)$ (due to `std::map` insertions).
*   **DFS Traversal:** $O(V + E) \approx O(N)$.
*   **CPU Move Selection:** $O(M \cdot N \log N)$ where $M$ is number of moves.
*   **DP Solver:** Exponential in the worst case, but reduced significantly by memoization and frontier-state pruning.
*   **Sorting:** $O(N \log N)$ for both Merge and Quick Sort.

**Space Complexity:**
*   **Grid Storage:** $O(N)$.
*   **Graph (Adjacency List):** $O(N)$.
*   **Recursion Stack (DFS/Recursive Enumeration):** $O(N)$.

---

### 8. OOP & DESIGN PATTERNS

**Why is a Player interface used?**
(Reflected in `Move` actors "HUMAN"/"CPU"). It allows the game loop to treat both entities uniformly.

**Which design pattern does it represent?**
**Strategy Pattern** (for CPU logic) and **MVC (Model-View-Controller)**.

---

### 9. GUI & INTEGRATION

**How does the GUI communicate with the game engine?**
**Inter-Process Communication (IPC) via Standard I/O.**
1.  Java serializes the `GameState` to **JSON**.
2.  Spawns `nets_engine.exe` and pipes the JSON to `stdin`.
3.  C++ engine processes (runs Greedy or Recursive Enumeration) and prints JSON to `stdout`.
4.  Java reads and deserializes the response.

---

### 10. TEST CASES & EDGE CASES

**What happens if the puzzle is unsolvable?**
The DP Solver will explore all possibilities and return `false`, allowing the system to notify the user.

**How are boundary conditions handled?**
The `GraphBuilder` and `DacSolver` explicitly check array bounds.

---

### 11. REAL-WORLD APPLICATIONS

**Where can these algorithms be applied?**
*   **Network Routing:** Finding paths in computer networks.
*   **Circuit Design:** Routing tracks on a PCB (Recursive Enumeration is common here).
*   **Scheduling:** Constraint Satisfaction Problems (CSPs).

---

### 12. LIMITATIONS & FUTURE SCOPE

**What are the limitations of this implementation?**
*   **Performance:** Spawning a process for every move (IPC) has overhead.
*   **Search Depth:** The Greedy AI is still "shallow" compared to a full Minimax approach.

**How can the project be further improved?**
*   Implement **Minimax with Alpha-Beta Pruning** for the CPU to plan multiple moves ahead.
*   Use **Bitmasks** to represent tile rotations for even faster connectivity checks.

---

### 13. FINAL DEFENSE CONCLUSION

**Why is this a strong academic DAA project?**
It bridges **theory** (Graphs, DFS, Quick Sort, Merge Sort, Recursive Enumeration) and **practice** (Hybrid Architecture, JSON/IPC, GUI). It demonstrates multiple algorithmic paradigms: **Greedy**, **Divide & Conquer**, and **State-Space Search (Recursive Enumeration)**.

**What makes this project unique?**
The use of two distinct sorting algorithms (Quick and Merge) for different optimization goals and the integration of a high-performance C++ solver with a JavaFX frontend.
