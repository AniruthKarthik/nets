# C++ Engine Workflow

1. **Input Phase**
    *   **File:** `cpp/JsonImporter.cpp`
    *   **Function:** `importGameState(filename)`
    *   **Input:** `json/board_state.json` (string path)
    *   **Output:** `GameState` struct (contains `Board`, `turn`, `status`)

2. **Analysis Phase (Validation)**
    *   **File:** `cpp/ConnectivityCheck.cpp`
    *   **Function:** `countLooseEnds(board)`, `countComponents(board)`, `hasClosedLoop(board)`
    *   **Input:** `Board` struct (from `GameState`)
    *   **Output:** Integer stats (loose ends, component count) and boolean flags (loop detected)

3. **Strategy Phase (CPU Turn)**
    *   **File:** `cpp/CpuStrategy.cpp`
    *   **Function:** `chooseBestMove(board)`
    *   **Input:** `Board` struct
    *   **Process:**
        *   Calls `generateMoves(board)` to list all possible rotations.
        *   Simulates each move on a temp board.
        *   Calls `evaluateBoard(tempBoard)` (uses `ConnectivityCheck` functions).
    *   **Output:** `Move` struct (x, y, rotation)

4. **Execution Phase**
    *   **File:** `cpp/GameLogic.cpp`
    *   **Function:** `applyMove(board, move)`
    *   **Input:** `Board` ref, `Move` struct
    *   **Output:** Updates `Board` state in-place (rotates tile)

5. **Win Check Phase**
    *   **File:** `cpp/ConnectivityCheck.cpp`
    *   **Function:** `isSolved(board)`
    *   **Input:** `Board` struct
    *   **Output:** `bool` (true if solved: 0 loose ends, 1 component, no loops)

6. **Output Phase**
    *   **File:** `cpp/JsonExporter.cpp`
    *   **Function:** `exportGameState(state, filename)`
    *   **Input:** Updated `GameState` struct
    *   **Output:** Writes to `json/board_state.json`
