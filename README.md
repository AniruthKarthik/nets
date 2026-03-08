# NETS: Network Connectivity Puzzle

## Project Overview
**NETS** is a hybrid puzzle game that combines a **JavaFX GUI** with a high-performance **C++ Computational Engine**. The goal is to rotate network tiles to connect all clients (PCs) to a central server (Power Source) without leaving loose ends or creating closed loops.

This project demonstrates the practical application of **Design and Analysis of Algorithms (DAA)** concepts, including **Graph Theory**, **Greedy Algorithms**, **Minimum Spanning Trees**, and **Complexity Analysis**.

---

## Key Features
*   **Hybrid Architecture:** Java for the interactive UI and C++ for heavy algorithmic processing.
*   **Procedural Level Generation:** Uses **Randomized Prim's Algorithm** to generate unique, solvable puzzles every time.
*   **High-Performance Engine:** Optimized C++ backend using bitwise operations, cache-friendly data structures (1D grid), and O(1) graph lookups.
*   **Advanced AI Visualizer:** A specialized mode to watch the AI's search process in real-time with smooth, directional animations.
*   **Multiple Solvers:** Includes **Backtracking (BT)**, **Dynamic Programming (DP)**, and **Divide & Conquer (D&C)** implementations.
*   **Real-time Graph Analysis:** Instant feedback on connectivity, loops, and loose ends using **DFS**.
*   **Dynamic Difficulty:** Supports board sizes from 3x3 up to 15x15.

---

## Tech Stack
*   **Frontend / UI:** Java 17+, JavaFX 21
*   **Backend / Engine:** C++17 (Highly optimized for cache locality and bitwise speed)
*   **Build Tools:** Maven (Java), Make/G++ (C++)
*   **Communication:** JSON-based Inter-Process Communication (IPC) via StdIO

---

## Recent Improvements & Optimizations

### 🚀 Engine Performance
*   **1D Grid Flattening:** The board representation was moved from a 2D `vector<vector<Tile>>` to a 1D `vector<Tile>`. This significantly improves cache hits and reduces memory allocation overhead during recursive solving.
*   **Bitwise Port Matching:** Replaced expensive `vector<Direction>` allocations with `uint8_t` bitmasks. Constraint checking now uses O(1) bitwise AND/SHIFT operations.
*   **Graph Optimization:** Replaced `std::map` and `std::set` in the Graph structure with flattened `std::vector` and `vector<bool>`, reducing graph traversal overhead from O(log V) to O(1).

### 🎨 UI & Visualization
*   **Directional Animations:** The AI Visualizer now uses intelligent rotation logic:
    *   **TRY** steps rotate **Clockwise (Right)** to show the search path.
    *   **UNDO** steps rotate **Counter-Clockwise (Left)** to show backtracking.
*   **Smooth Transitions:** Implemented JavaFX `Timeline` animations for all tile rotations, providing a polished and "alive" feel to the game.
*   **Polished Sidebar:** Optimized the step-details panel to prevent text truncation and provide clearer algorithm labeling (e.g., "DP" for Dynamic Programming).

---

## Project Structure

```
nets/
├── cpp/                   # C++ Engine Source Code
│   ├── GameLogic.hpp      # Core game rules
│   ├── GraphBuilder.hpp   # Adjacency List construction (Optimized)
│   ├── CpuStrategy.hpp    # Greedy AI logic
│   ├── DacSolver.hpp       # Divide-and-conquer global solver
│   ├── BtSolver.hpp        # BT solver (Backtracking)
│   ├── DpSolver.hpp        # DP solver (Dynamic Programming)
│   ├── SortUtils.hpp      # Merge Sort (D&C) for tiles
│   └── ConnectivityCheck.hpp # DFS & Cycle Detection (O(1) lookups)
├── netgame/               # Java Application Source Code
│   ├── pom.xml            # Maven dependencies
│   └── src/main/java/com/nets/
│       ├── controller/    # Game Logic & IPC Controller
│       ├── model/         # Data Models (GameState, Tile, VisualStep)
│       └── view/          # JavaFX GUI & Animation Components
├── nets_engine.cpp        # C++ Entry Point (Optimized main loop)
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
*   **Greedy Approach (CPU):** Evaluates possible rotations and picks the one that minimizes the **Heuristic Cost**.
*   **DP Solver:** Explores the state space using memoized recursion and a compact frontier state. Uses optimized bitmasks for ultra-fast constraint checking.
*   **D&C Solver:** Splits the board into sub-regions and combines them via cut-edge constraints.
*   **Visualizer Mode:** Records every "TRY" and "UNDO" operation from the engine and replays them in the UI with directional animations.

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
    *   All PCs are powered (green wires).
    *   No loose ends.
    *   No closed loops.

---

## Documentation
For a deeper dive into the architecture and execution flow, refer to:
*   **[WORKFLOW.md](workflow.md):** Step-by-step execution details from startup to termination.
*   **[QnA.md](qna.md):** Academic defense, algorithmic analysis, and complexity details.
