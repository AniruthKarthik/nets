# C++ Engine Workflow

The C++ engine is architected as a set of header-only libraries (`.hpp`) located in the `cpp/` directory, orchestrated by a main driver file `nets_engine.cpp`.

1. **Input Phase**
    *   **File:** `cpp/JsonImporter.hpp`
    *   **Function:** `importGameState(filename)`
    *   **Input:** `json/board_state.jsonc` (string path)
    *   **Output:** `GameState` struct (contains `Board`, `turn`, `status`)

2. **Analysis Phase (Validation)**
    *   **File:** `cpp/ConnectivityCheck.hpp`
    *   **Function:** `countLooseEnds(board)`, `countComponents(board)`, `hasClosedLoop(board)`
    *   **Input:** `Board` struct (from `GameState`)
    *   **Output:** Integer stats (loose ends, component count) and boolean flags (loop detected)

3. **Strategy Phase (CPU Turn)**
    *   **File:** `cpp/CpuStrategy.hpp`
    *   **Function:** `chooseBestMove(board)`
    *   **Input:** `Board` struct
    *   **Process:**
        *   Calls `generateMoves(board)` to list all possible rotations (respecting locked tiles).
        *   Simulates each move on a temp board.
        *   Calls `evaluateBoard(tempBoard)` (uses `ConnectivityCheck` functions).
    *   **Output:** `Move` struct (x, y, rotation)

4. **Execution Phase**
    *   **File:** `cpp/GameLogic.hpp`
    *   **Function:** `applyMove(board, move)`
    *   **Input:** `Board` ref, `Move` struct
    *   **Output:** Updates `Board` state in-place (rotates tile)

5. **Win Check Phase**
    *   **File:** `cpp/ConnectivityCheck.hpp`
    *   **Function:** `isSolved(board)`
    *   **Input:** `Board` struct
    *   **Output:** `bool` (true if solved: 0 loose ends, 1 component, no loops)

6. **Output Phase**
    *   **File:** `cpp/JsonExporter.hpp`
    *   **Function:** `exportGameState(state, filename)`
    *   **Input:** Updated `GameState` struct
    *   **Output:** Writes to `json/board_state.jsonc`

## compilation & Usage

*   **Main Engine:**
    ```bash
    g++ -std=c++17 nets_engine.cpp -o nets_engine
    ./nets_engine [input_file] [output_file]
    ```

*   **Testing:**
    ```bash
    g++ -std=c++17 testing/tester.cpp -o testing/tester
    ./testing/tester
    ```
