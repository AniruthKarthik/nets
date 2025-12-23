package com.nets.controller;

import com.nets.model.*;
import com.nets.view.GameBoard;
import com.nets.view.TileView;
import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.input.MouseButton;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.io.*;
import java.util.*;

public class GameController {
    private GameBoard gameBoard;
    private GameState gameState;

    public GameController(GameBoard gameBoard) {
        this.gameBoard = gameBoard;
    }

    public void initGame(int rows, int cols) {
        try {
            // Create new game state
            gameState = createNewGameState(rows, cols);
            gameBoard.loadGameState(gameState);
            setupEventHandlers();
            updatePoweredStatus();
        } catch (Exception e) {
            showError("Failed to initialize game: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private GameState createNewGameState(int rows, int cols) {
        GameState state = new GameState();

        // Initialize meta
        Meta meta = new Meta();
        meta.setWidth(cols);
        meta.setHeight(rows);
        meta.setStatus("PLAYING");
        meta.setTurn("HUMAN");
        meta.setSeed(new Random().nextInt());
        meta.setWraps(false);
        state.setMeta(meta);

        // 1. Generate a Spanning Tree starting from the center POWER tile
        Tile[][] grid = new Tile[rows][cols];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j] = new Tile(TileType.EMPTY, 0, false);
            }
        }

        int startR = rows / 2;
        int startC = cols / 2;
        grid[startR][startC] = new Tile(TileType.POWER, 0, true);

        // Prim's algorithm for spanning tree
        List<int[]> edges = new ArrayList<>();
        boolean[][] inTree = new boolean[rows][cols];
        inTree[startR][startC] = true;

        int[][] dirs = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        addEdges(edges, startR, startC, rows, cols, inTree);

        Random rand = new Random();
        boolean[][] connections = new boolean[rows * cols][4]; // [cellIndex][direction]

        while (!edges.isEmpty()) {
            int edgeIdx = rand.nextInt(edges.size());
            int[] edge = edges.remove(edgeIdx);
            int pr = edge[0], pc = edge[1], r = edge[2], c = edge[3], dir = edge[4];

            if (!inTree[r][c]) {
                inTree[r][c] = true;
                connections[pr * cols + pc][dir] = true;
                connections[r * cols + c][(dir + 2) % 4] = true;
                addEdges(edges, r, c, rows, cols, inTree);
            }
        }

        // 2. Assign TileTypes based on connections
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (r == startR && c == startC) continue;
                
                boolean[] conn = connections[r * cols + c];
                int count = 0;
                for (boolean b : conn) if (b) count++;

                if (count == 0) {
                    grid[r][c] = new Tile(TileType.EMPTY, 0, false);
                } else if (count == 1) {
                    // Leaf nodes are PCs
                    grid[r][c] = new Tile(TileType.PC, 0, false);
                    // Set rotation to match connection
                    for (int d = 0; d < 4; d++) if (conn[d]) grid[r][c].setRotation(d * 90);
                } else {
                    // Internal nodes are wires
                    assignWireType(grid[r][c], conn);
                }
            }
        }

        // Special case: POWER tile connections
        boolean[] pConn = connections[startR * cols + startC];
        grid[startR][startC].setConnections(pConn);
        // POWER is already locked and at 0 rotation. 
        // We set the base connections (at rotation 0) to be exactly what the tree generation produced.

        // 3. Scramble the puzzle (except POWER and EMPTY)
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (grid[r][c].getType() != TileType.POWER && grid[r][c].getType() != TileType.EMPTY) {
                    grid[r][c].setRotation(rand.nextInt(4) * 90);
                }
            }
        }

        state.setGrid(grid);

        // Initialize stats
        Stats stats = new Stats();
        stats.setComponents(calculateComponents(grid));
        stats.setLooseEnds(calculateLooseEnds(grid));
        stats.setSolved(false);
        state.setStats(stats);

        // Rules
        Rules rules = new Rules();
        rules.setAllowLoops(false);
        state.setRules(rules);

        return state;
    }

    private void addEdges(List<int[]> edges, int r, int c, int rows, int cols, boolean[][] inTree) {
        int[][] dirs = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        for (int i = 0; i < 4; i++) {
            int nr = r + dirs[i][0];
            int nc = c + dirs[i][1];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !inTree[nr][nc]) {
                edges.add(new int[]{r, c, nr, nc, i});
            }
        }
    }

    private void assignWireType(Tile tile, boolean[] conn) {
        int count = 0;
        for (boolean b : conn) if (b) count++;

        if (count == 2) {
            if (conn[0] && conn[2]) { tile.setType(TileType.STRAIGHT); tile.setRotation(0); }
            else if (conn[1] && conn[3]) { tile.setType(TileType.STRAIGHT); tile.setRotation(90); }
            else if (conn[0] && conn[1]) { tile.setType(TileType.CORNER); tile.setRotation(0); }
            else if (conn[1] && conn[2]) { tile.setType(TileType.CORNER); tile.setRotation(90); }
            else if (conn[2] && conn[3]) { tile.setType(TileType.CORNER); tile.setRotation(180); }
            else if (conn[3] && conn[0]) { tile.setType(TileType.CORNER); tile.setRotation(270); }
        } else if (count == 3) {
            tile.setType(TileType.T_JUNCTION);
            if (!conn[3]) tile.setRotation(0);      // N, E, S
            else if (!conn[0]) tile.setRotation(90); // E, S, W
            else if (!conn[1]) tile.setRotation(180);// S, W, N
            else tile.setRotation(270);             // W, N, E
        } else if (count == 4) {
            tile.setType(TileType.CROSS);
            tile.setRotation(0);
        }
    }

    private void setupEventHandlers() {
        TileView[][] tileViews = gameBoard.getTileViews();

        for (int row = 0; row < tileViews.length; row++) {
            for (int col = 0; col < tileViews[row].length; col++) {
                TileView tileView = tileViews[row][col];

                tileView.setOnMouseClicked(event -> {
                    if (gameState.getMeta().getTurn().equals("HUMAN") &&
                            gameState.getMeta().getStatus().equals("PLAYING")) {

                        Tile tile = tileView.getTile();
                        if (!tile.isLocked()) {
                            int rotation = event.getButton() == MouseButton.PRIMARY ? 90 : -90;
                            handleHumanMove(tileView.getRow(), tileView.getCol(), rotation);
                        }
                    }
                });

                tileView.setOnMouseEntered(event -> {
                    if (!tileView.getTile().isLocked() &&
                            gameState.getMeta().getTurn().equals("HUMAN") &&
                            gameState.getMeta().getStatus().equals("PLAYING")) {
                        tileView.setOpacity(0.8);
                        tileView.setStyle("-fx-cursor: hand;");
                    }
                });

                tileView.setOnMouseExited(event -> {
                    tileView.setOpacity(1.0);
                    tileView.setStyle("-fx-cursor: default;");
                });
            }
        }
    }

    private void handleHumanMove(int row, int col, int rotation) {
        try {
            // Update local state
            Tile tile = gameState.getGrid()[row][col];
            tile.rotate(rotation);

            // Update move
            Move move = new Move("HUMAN", row, col, tile.getRotation());
            gameState.setLastMove(move);

            // Recalculate stats and powered status
            updateStats();
            updatePoweredStatus();

            // Update UI
            for (int i = 0; i < gameState.getMeta().getHeight(); i++) {
                for (int j = 0; j < gameState.getMeta().getWidth(); j++) {
                    gameBoard.getTileView(i, j).updateTile(gameState.getGrid()[i][j]);
                }
            }
            gameBoard.updateUI();

            // Check if solved
            if (checkWinCondition()) {
                gameState.getMeta().setStatus("SOLVED");
                gameBoard.updateUI();
                showWinMessage();
                return;
            }

            // Switch to CPU turn
            gameState.getMeta().setTurn("CPU");
            gameBoard.updateUI();

            // Execute CPU turn
            new Thread(() -> {
                try {
                    Thread.sleep(500); // Small delay for visual effect
                    Platform.runLater(this::performStandaloneCpuMove);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }).start();

        } catch (Exception e) {
            showError("Error processing move: " + e.getMessage());
        }
    }

    private void performStandaloneCpuMove() {
        try {
            Gson gson = new GsonBuilder().create();
            
            // Locate C++ engine
            File engineExe = new File("..\\nets_engine.exe");
            if (!engineExe.exists()) {
                engineExe = new File("nets_engine.exe");
            }
            
            if (!engineExe.exists()) {
                 throw new FileNotFoundException("Could not find nets_engine.exe at " + new File("..\\nets_engine.exe").getAbsolutePath() + " or " + new File("nets_engine.exe").getAbsolutePath());
            }

            // Call C++ engine
            ProcessBuilder pb = new ProcessBuilder(engineExe.getCanonicalPath());
            // pb.directory(new File(".")); // Inherit current working directory
            Process process = pb.start();

            // Write current state to stdin
            try (OutputStreamWriter writer = new OutputStreamWriter(process.getOutputStream())) {
                gson.toJson(gameState, writer);
            }

            // Read response from stdout
            StringBuilder output = new StringBuilder();
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    output.append(line);
                }
            }

            int exitCode = process.waitFor();
            if (exitCode != 0) {
                 // Try reading stderr if failed
                 try (BufferedReader errReader = new BufferedReader(new InputStreamReader(process.getErrorStream()))) {
                    String line;
                    while((line = errReader.readLine()) != null) System.err.println("CPP Error: " + line);
                 }
                 throw new RuntimeException("C++ engine exited with code " + exitCode);
            }

            // Parse response: { "move": { "row": ..., "col": ..., "rotation": ... } }
            com.google.gson.JsonObject response = gson.fromJson(output.toString(), com.google.gson.JsonObject.class);
            if (response != null && response.has("move")) {
                com.google.gson.JsonObject moveObj = response.getAsJsonObject("move");
                int r = moveObj.get("row").getAsInt();
                int c = moveObj.get("col").getAsInt();
                int rot = moveObj.get("rotation").getAsInt();

                // Apply move
                Tile tile = gameState.getGrid()[r][c];
                tile.setRotation(rot); // Absolute rotation
                
                gameState.setLastMove(new Move("CPU", r, c, rot));

                // Recalculate stats
                updateStats();
                updatePoweredStatus();
            } else {
                System.err.println("Invalid response from CPU: " + output);
            }

            // Update UI
            Platform.runLater(() -> {
                for (int i = 0; i < gameState.getMeta().getHeight(); i++) {
                    for (int j = 0; j < gameState.getMeta().getWidth(); j++) {
                        gameBoard.getTileView(i, j).updateTile(gameState.getGrid()[i][j]);
                    }
                }
                updatePoweredStatus();
                gameBoard.updateUI();
                
                // Switch back to human
                gameState.getMeta().setTurn("HUMAN");
                gameBoard.updateUI();

                checkAndHandleWin();
            });

        } catch (Exception e) {
            e.printStackTrace();
            showError("CPU Error: " + e.getMessage());
            gameState.getMeta().setTurn("HUMAN");
            gameBoard.updateUI();
        }
    }

    private void updateStats() {
        Tile[][] grid = gameState.getGrid();
        Stats stats = gameState.getStats();
        stats.setComponents(calculateComponents(grid));
        stats.setLooseEnds(calculateLooseEnds(grid));
        stats.setSolved(checkWinCondition());
    }

    private int calculateLooseEnds(Tile[][] grid) {
        int looseEnds = 0;
        int rows = grid.length;
        int cols = grid[0].length;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Tile tile = grid[i][j];
                if (tile.getType() == TileType.EMPTY) continue;

                boolean[] connections = getConnections(tile);

                // Check each direction
                for (int dir = 0; dir < 4; dir++) {
                    if (connections[dir]) {
                        int ni = i + (dir == 0 ? -1 : dir == 2 ? 1 : 0);
                        int nj = j + (dir == 1 ? 1 : dir == 3 ? -1 : 0);

                        if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) {
                            looseEnds++;
                        } else {
                            Tile neighbor = grid[ni][nj];
                            boolean[] neighborConn = getConnections(neighbor);
                            int oppositeDir = (dir + 2) % 4;

                            if (!neighborConn[oppositeDir]) {
                                looseEnds++;
                            }
                        }
                    }
                }
            }
        }

        return looseEnds / 2; // Each loose end counted twice
    }

    private int calculateComponents(Tile[][] grid) {
        int rows = grid.length;
        int cols = grid[0].length;
        boolean[][] visited = new boolean[rows][cols];
        int components = 0;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (!visited[i][j] && grid[i][j].getType() != TileType.EMPTY) {
                    dfs(grid, visited, i, j, null);
                    components++;
                }
            }
        }

        return components;
    }

    private void dfs(Tile[][] grid, boolean[][] visited, int i, int j, Set<Tile> poweredSet) {
        if (i < 0 || i >= grid.length || j < 0 || j >= grid[0].length) return;
        if (visited[i][j] || grid[i][j].getType() == TileType.EMPTY) return;

        visited[i][j] = true;
        if (poweredSet != null) {
            poweredSet.add(grid[i][j]);
        }
        
        // PCs are sinks; they do not propagate power further.
        if (grid[i][j].getType() == TileType.PC) {
            return;
        }

        boolean[] connections = getConnections(grid[i][j]);

        int[][] dirs = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        for (int d = 0; d < 4; d++) {
            if (connections[d]) {
                int ni = i + dirs[d][0];
                int nj = j + dirs[d][1];

                if (ni >= 0 && ni < grid.length && nj >= 0 && nj < grid[0].length) {
                    Tile neighbor = grid[ni][nj];
                    boolean[] neighborConn = getConnections(neighbor);
                    if (neighborConn[(d + 2) % 4]) {
                        dfs(grid, visited, ni, nj, poweredSet);
                    }
                }
            }
        }
    }

    private void updatePoweredStatus() {
        Tile[][] grid = gameState.getGrid();
        int rows = grid.length;
        int cols = grid[0].length;

        // Reset all tiles to unpowered
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j].setPowered(false);
            }
        }

        // Find power source
        int powerRow = -1, powerCol = -1;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j].getType() == TileType.POWER) {
                    powerRow = i;
                    powerCol = j;
                    break;
                }
            }
        }

        if (powerRow != -1) {
            // Traverse from power source to find all connected tiles
            Set<Tile> poweredSet = new HashSet<>();
            boolean[][] visited = new boolean[rows][cols];
            dfs(grid, visited, powerRow, powerCol, poweredSet);
            for(Tile t: poweredSet) {
                t.setPowered(true);
            }
        }
    }


    private boolean[] getConnections(Tile tile) {
        // Returns [top, right, bottom, left]
        if (tile.getConnections() != null) {
            // If custom connections are set (e.g. for POWER), rotate them
            boolean[] base = tile.getConnections();
            boolean[] rotated = new boolean[4];
            int shift = tile.getRotation() / 90;
            for (int i = 0; i < 4; i++) {
                rotated[(i + shift) % 4] = base[i];
            }
            return rotated;
        }

        boolean[] conn = new boolean[4];
        int rot = tile.getRotation();

        switch (tile.getType()) {
            case STRAIGHT:
                // [T, F, T, F] -> N, S at rot 0
                if (rot == 0 || rot == 180) { conn[0] = conn[2] = true; }
                else { conn[1] = conn[3] = true; }
                break;
            case CORNER:
                // [T, T, F, F] -> N, E at rot 0
                if (rot == 0) { conn[0] = conn[1] = true; }
                else if (rot == 90) { conn[1] = conn[2] = true; }
                else if (rot == 180) { conn[2] = conn[3] = true; }
                else { conn[3] = conn[0] = true; }
                break;
            case T_JUNCTION:
                // [T, T, T, F] -> N, E, S at rot 0
                if (rot == 0) { conn[0] = conn[1] = conn[2] = true; }
                else if (rot == 90) { conn[1] = conn[2] = conn[3] = true; }
                else if (rot == 180) { conn[2] = conn[3] = conn[0] = true; }
                else { conn[3] = conn[0] = conn[1] = true; }
                break;
            case PC:
                // [T, F, F, F] -> N at rot 0
                if (rot == 0) { conn[0] = true; }
                else if (rot == 90) { conn[1] = true; }
                else if (rot == 180) { conn[2] = true; }
                else { conn[3] = true; }
                break;
            case POWER:
            case CROSS:
                // Power and Cross behave as 4-way junctions.
                conn[0] = conn[1] = conn[2] = conn[3] = true;
                break;
        }

        return conn;
    }

    private boolean checkWinCondition() {
        Tile[][] grid = gameState.getGrid();
        int rows = grid.length;
        int cols = grid[0].length;

        // 1. Check loose ends and components
        if (calculateLooseEnds(grid) != 0 || calculateComponents(grid) != 1) {
            return false;
        }

        // 2. Check if all PCs and non-EMPTY tiles are powered
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Tile tile = grid[i][j];
                if (tile.getType() == TileType.EMPTY) continue;
                
                if (!tile.isPowered()) {
                    return false;
                }
            }
        }
        return true;
    }

    private void checkAndHandleWin() {
        if (checkWinCondition()) {
            gameState.getMeta().setStatus("SOLVED");
            gameBoard.updateUI();
            showWinMessage();
        }
    }

    private void showWinMessage() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("Victory!");
        alert.setHeaderText("🎉 Congratulations! 🎉");
        alert.setContentText("You've successfully connected all PCs to the power source!\n\nAll tiles are connected in a single network.");
        alert.showAndWait();
    }

    private void showError(String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Error");
        alert.setHeaderText("An error occurred");
        alert.setContentText(message);
        alert.showAndWait();
    }

    public void resetGame(int rows, int cols) {
        initGame(rows, cols);
    }
}