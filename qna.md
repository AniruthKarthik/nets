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
*   **Sorting:** Quick Sort is implemented to prioritize move evaluation.
*   **Heuristics:** A scoring function to evaluate the "goodness" of a game state.

**How is this project relevant to Design and Analysis of Algorithms (DAA)?**
It demonstrates the practical application of theoretical DAA concepts like **time complexity analysis**, **graph data structures** (Adjacency Lists), **recursion** (DFS, QuickSort), and **optimization strategies** (Greedy approach) in a real-world software system.

**Project Summary:**
A hybrid C++/Java puzzle game that utilizes graph algorithms and greedy strategies to solve network connectivity problems in real-time.

---

### 2. GAME & APPROACH

**Why is a game-based approach suitable here?**
A game provides an interactive visual representation of abstract graph algorithms. It allows users to "see" connectivity and cycles, making complex algorithmic states (like "Disjoint Sets" or "Cyclic Graphs") intuitively understandable.

**Is the game turn-based? Why or why not?**
**Yes.** The game implements a turn-based system (Human vs. CPU). This allows the C++ engine to perform computationally intensive calculations (analyzing thousands of potential board states) between user moves without UI freezing.

**How does scoring work in this version of NETS?**
While there isn't a traditional "score," the game state is evaluated using a **cost function** (implemented in `CpuStrategy.hpp`):
`Score = (LooseEnds * 10) + (Components * 5) + (HasLoop ? 1000 : 0)`
The goal is to minimize this score. A score of 0 implies a solved state (ideally).

**What happens when a risky / losing move is made?**
If a move creates a **closed loop** (cycle), the heuristic function assigns a massive penalty (`1000` points). This discourages the CPU (and warns the player implicitly via game feedback) from making moves that violate the spanning tree structure.

**How is a winner or game-ending condition determined?**
The game ends with a "Victory" when:
1.  **Zero Loose Ends:** No wires pointing to empty space or mismatched connections.
2.  **Single Component:** All tiles are connected in one graph (verified via DFS).
3.  **No Loops:** The graph is acyclic.
4.  **All Powered:** Connectivity traces back to the central `POWER` tile.

**What limitations exist in the current game logic?**
The CPU uses a **1-step lookahead Greedy Strategy**. It evaluates the immediate best move but does not plan multiple turns ahead (like Minimax). Therefore, it can theoretically get stuck in a "local optimum" where no single move improves the score, even if the puzzle is solvable.

---

### 3. ALGORITHMS USED

**Which algorithms are implemented?**
1.  **Depth First Search (DFS):** For traversing the graph to count connected components and detect cycles.
2.  **Randomized Prim's Algorithm:** Used in `GameController.java` to generate a perfect maze (Spanning Tree) for the level.
3.  **Greedy Best-First Search:** Used by the CPU to pick the optimal move.
4.  **Quick Sort:** A custom implementation to sort potential moves by tile priority.

**Where is graph traversal used?**
*   **Cycle Detection:** `hasClosedLoop` in `ConnectivityCheck.hpp` uses DFS to find back-edges.
*   **Component Counting:** `countComponents` uses iterative DFS to find the number of disjoint subgraphs.
*   **Power Propagation:** Java's `updatePoweredStatus` uses DFS to light up connected cables.

**Where is a greedy strategy applied?**
In `CpuStrategy.hpp`, the function `chooseBestMove`:
1.  Generates all possible rotations for every unlocked tile.
2.  Simulates each move on a temporary board.
3.  Calculates the "heuristic score" (cost).
4.  **Greedily** selects the move with the lowest cost.

**Is sorting used? If yes, where and why?**
**Yes, Quick Sort.** Implemented in `CpuStrategy.hpp` (`quickSort_recursive`).
It sorts the list of potential moves based on **Tile Priority**:
`T_JUNCTION (4) > CORNER (3) > STRAIGHT (2) > PC (1)`.
This ensures the CPU evaluates complex tiles (which constrain the board most) before simple ones.

**Why was a greedy approach selected?**
A Greedy approach is computationally efficient ($O(N)$ moves to evaluate) compared to a full brute-force backtracking solver ($O(3^N)$), making it suitable for a responsive real-time game.

**Is the greedy strategy always optimal in this game?**
**No.** Greedy algorithms make the locally optimal choice at each stage. In complex puzzle configurations, this might not lead to the global optimum (the solution), potentially requiring the user to intervene.

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
An adjacency list is space-efficient $O(V+E)$ for sparse graphs like this grid (where max degree is 4). An Adjacency Matrix would be $O(V^2)$, which is wasteful.

**Why were HashMap (Map), ArrayList (Vector), or arrays used?**
*   **`std::map`**: Used in C++ to map coordinates to neighbor lists. It allows flexible lookup of sparse nodes.
*   **`std::vector`**: Dynamic arrays used for lists of moves and neighbors, offering $O(1)$ access and cache friendliness.
*   **Java Arrays (`Tile[][]`)**: Used for the visual grid representation because the board has fixed dimensions.

**What is the maximum degree of a node?**
**4**. A tile can connect to North, East, South, and West.

**How are edge and boundary cells handled?**
The `getNeighbor` function checks bounds `(0 <= r < height)` and `(0 <= c < width)`. If a connection points off-grid, it is ignored (returns `{-1, -1}`), preventing IndexOutOfBounds errors.

---

### 5. CPU / AI LOGIC

**How does the CPU decide its move?**
1.  **Generate:** List all legal rotations.
2.  **Sort:** Prioritize critical tiles (Junctions).
3.  **Simulate:** Apply move -> Build Graph -> Evaluate Metric.
4.  **Select:** Pick the move resulting in the lowest metric.

**How does the CPU calculate risk?**
It explicitly checks for **Closed Loops** using DFS. If a move creates a cycle, the `evaluateBoard` function returns a score > 1000, effectively marking it as a "bad move" unless no other options exist.

**Does the CPU use any hidden or unfair information?**
**No.** The CPU analyzes the current board state exactly as the player sees it. It does not peek at the "solution" generated during initialization.

**Why is the CPU logic considered fair?**
It adheres to the same rules as the player and relies on a deterministic algorithm (Greedy + Heuristics) rather than cheating.

---

### 6. SORTING

**Why was this sorting algorithm used?**
**Quick Sort** was implemented manually (`CpuStrategy.hpp`). It is generally efficient with $O(N \log N)$ performance.

**Why not built-in library sorting?**
Writing a custom Quick Sort demonstrates an understanding of the algorithm's divide-and-conquer mechanics (partitioning, recursion), which is a key requirement for DAA projects.

**Time complexity of the sorting algorithm:**
*   **Average Case:** $O(N \log N)$
*   **Worst Case:** $O(N^2)$ (if pivot selection is poor, though rare here).

**Space complexity of the sorting algorithm:**
*   $O(\log N)$ stack space for recursion.

---

### 7. TIME & SPACE COMPLEXITY

**Time Complexity:**
*   **Initialization (Prim's):** $O(N^2)$ (dominated by `ArrayList::remove` in the randomized selection).
*   **Graph Build:** $O(N \log N)$ (due to `std::map` insertions).
*   **DFS Traversal:** $O(V + E) \approx O(N)$.
*   **CPU Move Selection:**
    *   Generates $O(N)$ moves.
    *   For each move, builds graph ($O(N \log N)$) and runs DFS ($O(N)$).
    *   **Total:** $O(N^2 \log N)$.
*   **Quick Sort:** $O(N \log N)$.

**Space Complexity:**
*   **Grid Storage:** $O(N)$ ($N$ = total tiles).
*   **Graph (Adjacency List):** $O(N)$ (storing edges for sparse grid).
*   **Recursion Stack (DFS):** $O(N)$ (worst case: line graph).

---

### 8. OOP & DESIGN PATTERNS

**Why is a Player interface used?**
(Reflected in `Move` actors "HUMAN"/"CPU"). It allows the game loop to treat both entities uniformly, enabling extensibility (e.g., adding a "Network Player").

**Which design pattern does it represent?**
**Strategy Pattern** (for CPU logic) and **MVC (Model-View-Controller)**.

**Why is game logic separated from GUI?**
**MVC Pattern.**
*   **Model (`GameState`, C++ Engine):** Manages data and rules.
*   **View (`GameBoard`, JavaFX):** Displays the grid.
*   **Controller (`GameController`):** Handles input and bridges Model-View.
This separation makes the code modular, testable, and easier to maintain.

**What is the Move class?**
A Data Transfer Object (DTO) that encapsulates a state change: `(Actor, Row, Col, Rotation)`.

**Why are Move fields immutable?**
To ensure the history of moves (if tracked) or the state transition cannot be tampered with after creation, ensuring data integrity.

---

### 9. GUI & INTEGRATION

**Does the GUI contain business logic?**
**No.** The GUI (`TileView`, `GameBoard`) is purely for display. It reflects the state provided by the `GameController`.

**How does the GUI communicate with the game engine?**
**Inter-Process Communication (IPC) via Standard I/O.**
1.  Java serializes the `GameState` to **JSON**.
2.  Spawns `nets_engine.exe` and pipes the JSON to `stdin`.
3.  C++ engine processes and prints the result (JSON) to `stdout`.
4.  Java reads and deserializes the response.

**How does the game notify the GUI of changes?**
The `GameController` updates the `TileView` objects explicitly after a state change (`updateUI()`, `Platform.runLater()`).

---

### 10. TEST CASES & EDGE CASES

**What happens on the first move?**
The game engine initializes, calculates the initial connectivity stats (usually 1 component if generated correctly, but rotation scrambles it), and waits for input.

**What if the CPU makes a losing move?**
The game does not stop, but the heuristic score will spike. The player can correct it on their turn.

**What if a cell is clicked repeatedly?**
The `Tile` rotates 90 degrees per click. The state updates cyclically (0 -> 90 -> 180 -> 270 -> 0).

**How are boundary conditions handled?**
The `GraphBuilder` explicitly checks array bounds. A tile at `(0,0)` will not attempt to access `(-1, 0)`.

---

### 11. REAL-WORLD APPLICATIONS

**Where can greedy + heuristic strategies be applied?**
*   **Network Routing:** Finding paths in computer networks (OSPF).
*   **Circuit Design:** routing tracks on a PCB.
*   **Logistics:** Vehicle Routing Problem (VRP).

**How is this different from machine learning?**
This uses **Deterministic Algorithms** (explicit rules/math), whereas ML uses **Probabilistic Models** trained on data. This logic is interpretable and predictable.

---

### 12. LIMITATIONS & FUTURE SCOPE

**What are the limitations of this implementation?**
*   **Greedy Myopia:** The AI does not plan ahead.
*   **Performance:** Spawning a process for every move (IPC) is slower than JNI or a native socket connection.

**How can the CPU logic be improved?**
*   Implement **Minimax with Alpha-Beta Pruning** for deeper lookahead.
*   Add a **Backtracking Solver** to guarantee a solution if the Greedy approach fails.

**Can this support multiplayer?**
Yes, the underlying JSON serialization allows the game state to be easily sent over a network socket.

---

### 13. FINAL DEFENSE CONCLUSION

**Why is this a strong academic DAA project?**
It successfully bridges the gap between **theory** (Graphs, DFS, Sorting, Complexity) and **practice** (Software Engineering, IPC, GUI). It is not just a theoretical implementation but a working, interactive product.

**What makes this project unique?**
The **Hybrid Architecture** (Java + C++) is a standout feature, demonstrating the ability to integrate different technology stacks to leverage the strengths of each (Java for UI, C++ for speed).

**What key concepts were learned?**
*   Implementing Graph Data Structures from scratch.
*   Algorithm Complexity Analysis in practice.
*   System Integration via JSON/IPC.
*   Event-driven programming.