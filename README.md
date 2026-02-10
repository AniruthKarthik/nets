# NETS: Network Connectivity Puzzle

## Project Overview
**NETS** is a hybrid puzzle game that combines a **JavaFX GUI** with a high-performance **C++ Computational Engine**. The goal is to rotate network tiles to connect all clients (PCs) to a central server (Power Source) without leaving loose ends or creating closed loops.

This project demonstrates the practical application of **Design and Analysis of Algorithms (DAA)** concepts, including **Graph Theory**, **Greedy Algorithms**, **Minimum Spanning Trees**, and **Complexity Analysis**.

---

## Key Features
*   **Hybrid Architecture:** Java for the interactive UI and C++ for heavy algorithmic processing.
*   **Procedural Level Generation:** Uses **Randomized Prim's Algorithm** to generate unique, solvable puzzles every time.
*   **Smart CPU Opponent:** An AI player that uses a **Greedy Strategy** with heuristics to solve the puzzle.
*   **Backtracking Solver:** A high-performance solver that uses **State-Space Search** and **Merge Sort (D&C)** to find guaranteed solutions for any level.
*   **Real-time Graph Analysis:** Instant feedback on connectivity, loops, and loose ends using **DFS**.
*   **Dynamic Difficulty:** Supports board sizes from 3x3 up to 15x15.

---

## Tech Stack
*   **Frontend / UI:** Java 17+, JavaFX 21
*   **Backend / Engine:** C++17 (optimized for speed)
*   **Build Tools:** Maven (Java), Make/G++ (C++)
*   **Communication:** JSON-based Inter-Process Communication (IPC) via StdIO

---

## Project Structure

```
nets/
├── cpp/                   # C++ Engine Source Code
│   ├── GameLogic.hpp      # Core game rules
│   ├── GraphBuilder.hpp   # Adjacency List construction
│   ├── CpuStrategy.hpp    # Greedy AI logic (QuickSort implemented here)
│   ├── BacktrackingSolver.hpp # Recursive global solver
│   ├── SortUtils.hpp      # Merge Sort (Divide & Conquer) for tiles
│   └── ConnectivityCheck.hpp # DFS & Cycle Detection
├── netgame/               # Java Application Source Code
│   ├── pom.xml            # Maven dependencies
│   └── src/main/java/com/nets/
│       ├── controller/    # Game Logic & IPC Controller
│       ├── model/         # Data Models (GameState, Tile)
│       └── view/          # JavaFX GUI Components
├── nets_engine.cpp        # C++ Entry Point
├── Makefile               # C++ Build Script
├── README.md              # Project Documentation
├── qna.md                 # Detailed Algorithm Analysis
└── workflow.md            # Execution Flow Documentation
```

---

## Algorithms & Logic

### 1. Level Generation (Java)
*   **Randomized Prim's Algorithm:** Generates a perfect Spanning Tree (no cycles, fully connected) from the center outwards. This ensures every puzzle has at least one valid solution.

### 2. Graph Connectivity (C++)
*   **Depth First Search (DFS):** Used to traverse the grid and count:
    *   **Connected Components:** Should be 1 for a win.
    *   **Loose Ends:** Wires pointing to empty space or mismatched ports.
    *   **Cycles:** Closed loops, which are forbidden.

### 3. CPU & Solver Strategies (C++)
*   **Greedy Approach (CPU):** Evaluates possible rotations and picks the one that minimizes the **Heuristic Cost**. It uses **Quick Sort** to prioritize moves.
*   **Backtracking Solver:** Explores the state space using recursion and backtracking. It uses a **Divide and Conquer (Merge Sort)** strategy to sort tiles by constraint priority, significantly pruning the search space.

---

## Prerequisites

*   **Java Development Kit (JDK) 17** or higher.
*   **Maven** 3.x installed and added to PATH.
*   **G++ Compiler** (supporting C++17).
*   **Make** build tool.

---

## Build & Run Instructions

The project includes a `Makefile` that handles both the C++ compilation and the Java launch.

### 1. Compile C++ Engine
First, compile the high-performance engine:
```bash
make
# Generates nets_engine.exe
```

### 2. Run the Game
This command will compile the Java application and launch the GUI:
```bash
make game
```

### 3. Clean Build
To remove compiled binaries and temporary files:
```bash
make clean
```

---

## How to Play
1.  **Launch the Game:** Select your grid size (e.g., 5x5).
2.  **Objective:** Rotate tiles to create a single connected network powering all PCs.
3.  **Controls:**
    *   **Left Click:** Rotate a tile 90° clockwise.
    *   **Locked Tiles:** The Power Source (Server) is locked and cannot be rotated.
4.  **Win Condition:**
    *   All PCs are powered (yellow wires).
    *   No loose ends.
    *   No closed loops.

---

## Documentation
For a deeper dive into the architecture and execution flow, refer to:
*   **[WORKFLOW.md](workflow.md):** Step-by-step execution details from startup to termination.
*   **[QnA.md](qna.md):** Academic defense, algorithmic analysis, and complexity details.

---

## Future Scope
*   **Advanced AI:** Implementing Minimax or Alpha-Beta pruning for multi-turn planning.
*   **Multiplayer:** Enabling network play via Sockets.
*   **Optimization:** Replacing IPC with JNI (Java Native Interface) for zero-latency calls.
*   **Mobile Port:** Exporting the core logic to Android/iOS using Kotlin Multiplatform.