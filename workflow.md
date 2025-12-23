# NETS Game - Application Workflow

## 1. ENTRY POINT & PROGRAM STARTUP

*   **Main Entry File:** `netgame/src/main/java/com/nets/Launcher.java`
*   **Execution Start:** The program begins in the `main` method of the `Launcher` class.
*   **Immediate Action:** It calls `Application.launch(NetsGame.class, args)`, which initializes the JavaFX runtime.
*   **Object Creation:**
    1.  The JavaFX framework instantiates the `NetsGame` class.
    2.  The `start(Stage primaryStage)` method is invoked.
    3.  Inside `start`, a `Startup Dialog` is displayed to request board dimensions (rows, columns).
*   **Transition to Gameplay:** Once the user confirms dimensions (or cancels to accept defaults), `NetsGame.initializeGame()` is called to build the UI and Controller.

---

## 2. INITIALIZATION FLOW

*   **Grid Initialization:** The `initializeGame()` method in `NetsGame.java` instantiates `GameController` and calls `controller.initGame(rows, cols)`.
*   **Grid Structure:** The grid is a 2D array of `Tile` objects (`Tile[][]`), managed within a `GameState` object.
*   **Process & Validation:**
    1.  `GameController.createNewGameState()` calls `generateGrid()`.
    2.  `generateGrid` uses **Randomized Prim's Algorithm** to create a perfect spanning tree (guaranteed solvable).
    3.  Tiles are instantiated as `TileType.EMPTY`, `POWER`, `STRAIGHT`, `CORNER`, etc., based on the generated connections.
    4.  The grid is then "scrambled" (tiles randomly rotated) before being returned.
*   **Return Value:** A fully initialized `GameState` object containing the 2D grid, metadata, and rules.
*   **Next Step:** This state is passed to `GameBoard.loadGameState()` to generate the visual `TileView` components.

---

## 3. FUNCTION CALL CHAIN (CORE WORKFLOW)

**Typical Turn Execution:**
1.  **Event:** `TileView` (mouse click) ➔ `GameController.handleHumanMove()`.
2.  **Update State:** `handleHumanMove` modifies `Tile.rotation` in the 2D array.
3.  **Calculate Stats:** `handleHumanMove` ➔ `invokeCppEngine("get_stats")` ➔ `nets_engine.exe`.
4.  **Visual Feedback:** `handleHumanMove` ➔ `updatePoweredStatus()` (Java DFS) ➔ `TileView.updateTile()`.
5.  **Check Win:** `handleHumanMove` ➔ `checkWinCondition()`.
6.  **Next Turn:** If game continues ➔ `Meta.setTurn("CPU")` ➔ `performStandaloneCpuMove()`.

---

## 4. GAME LOOP FLOW

*   **Control Mechanism:** The game is **Event-Driven**. There is no traditional "while(true)" loop blocking the main thread.
*   **Turn Alternation:**
    *   **Human:** The application waits for UI events (mouse clicks).
    *   **CPU:** Triggered programmatically via a background thread after the human turn completes.
*   **Running Condition:** The game accepts input as long as `Meta.status` is "PLAYING".
*   **Stop Condition:** The game enters a "SOLVED" state when `checkWinCondition()` evaluates to true, disabling further input.

---

5. USER INPUT FLOW

*   **Origin:** Mouse Click on a Grid Cell (`TileView`).
*   **Receiver:** The event handler defined in `GameController.setupEventHandlers()`.
*   **Validation:**
    *   Check: Is the tile `locked`? (Power source is immutable).
    *   Check: Is `Meta.turn` == "HUMAN"?
    *   Check: Is `Meta.status` == "PLAYING"?
*   **Conversion:** The click coordinates `(row, col)` are mapped to the corresponding `Tile` object in the `GameState` grid.
*   **Application:** `Tile.rotate(90)` is called to update the model.

---

## 6. CPU / AI MOVE FLOW

*   **Trigger:** Automatically called via `Platform.runLater` inside a new Thread after `handleHumanMove` finishes.
*   **Selection:** `GameController` invokes the C++ engine (`nets_engine.exe`) with action `get_cpu_move`.
*   **Data Structure:** The C++ engine receives the full `GameState` as JSON. It builds an internal `Graph` (Adjacency List).
*   **Evaluation:**
    *   **Sorting:** `CpuStrategy.hpp` sorts moves by tile priority (Junction > Corner > Straight).
    *   **Greedy Search:** Iterates through sorted moves, applies them tentatively, and checks the "Board Score" (Loose Ends + Components).
*   **Application:** The best move (row, col, rotation) is returned to Java, which applies it to the `GameState` and updates the UI.

---

## 7. DATA FLOW SUMMARY

*   **Grid Movement:**
    *   **Java (Model):** `Tile[][]` in `GameState` is the authoritative source.
    *   **Java (View):** `TileView[][]` mirrors the model for display.
    *   **C++ (Engine):** Receives a JSON snapshot, builds a temporary `Board` and `Graph`, processes, and discards.
*   **State Updates:** Mutable `Tile` objects are modified in place. The `GameState` container is persistent.
*   **Consistency:** Java manages the "Truth". The C++ engine is a stateless function (Input State -> Output Move).

---

## 8. GUI ↔ GAME ENGINE INTERACTION

*   **Trigger:** User interacts with JavaFX Stage.
*   **Logic:** `GameController` processes the logic.
*   **Updates:**
    *   `GameController` updates `Tile` properties.
    *   `GameController` calls `gameBoard.getTileView(r, c).updateTile()`.
    *   `GameBoard` refreshes the JavaFX Scene Graph.
*   **Screen Refresh:** Handled automatically by the JavaFX Application Thread when nodes are modified.

---

## 9. ERROR HANDLING & EDGE FLOW

*   **Invalid Input:** Clicks on locked tiles or during CPU turns are silently ignored (event handlers return early).
*   **IPC Error:** If `nets_engine.exe` fails to launch or returns invalid JSON, `invokeCppEngine` throws an exception. `GameController` catches this and displays an **Alert Dialog** to the user, ensuring the app doesn't crash.
*   **Boundary Cases:**
    *   **Grid Edges:** `getNeighbor` functions check array bounds before access.
    *   **Empty State:** `GameState` is initialized with safe defaults to prevent NullPointerExceptions before the grid is generated.

---

## 10. FINAL TERMINATION FLOW

*   **End Condition:**
    1.  **Connectivity:** All non-empty tiles are powered (connected to Source).
    2.  **Graph Integrity:** Single Component, No Loose Ends, No Loops (verified by C++ engine stats).
*   **Detection:** `checkWinCondition()` runs after every move.
*   **Calculation:** Boolean AND of `isMathematicallySolved` (C++) and `areAllTilesPowered` (Java).
*   **Termination:**
    *   `Meta.status` becomes "SOLVED".
    *   `showWinMessage()` displays a Victory Dialog.
    *   Controls remain active only for "Reset" or "New Game".