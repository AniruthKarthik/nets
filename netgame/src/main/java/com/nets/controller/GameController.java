package com.nets.controller;

import com.nets.model.*;
import com.nets.util.JsonHandler;
import com.nets.view.GameBoard;
import com.nets.view.TileView;
import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.input.MouseButton;

import java.io.File;
import java.io.IOException;
import java.util.*;

public class GameController {
    private String jsonFile;
    private GameBoard gameBoard;
    private GameState gameState;
    private boolean cppEngineAvailable;

    public GameController(GameBoard gameBoard) {
        this.gameBoard = gameBoard;
        setupJsonPath();
        checkCppEngine();
    }

    private void setupJsonPath() {
        // Try multiple possible locations
        String[] possiblePaths = {
                "json/board_state.jsonc",
                "./json/board_state.jsonc",
                "../json/board_state.jsonc",
                System.getProperty("user.dir") + "/json/board_state.jsonc",
                System.getProperty("user.home") + "/nets-game/json/board_state.jsonc"
        };

        for (String path : possiblePaths) {
            File file = new File(path);
            if (file.getParentFile() != null && (file.getParentFile().exists() || file.getParentFile().mkdirs())) {
                jsonFile = path;
                System.out.println("Using JSON path: " + file.getAbsolutePath());
                return;
            }
        }

        // Default fallback - create in user home
        jsonFile = System.getProperty("user.home") + "/nets-game/json/board_state.jsonc";
        File file = new File(jsonFile);
        file.getParentFile().mkdirs();
        System.out.println("Created JSON path: " + file.getAbsolutePath());
    }

    private void checkCppEngine() {
        File engine = new File("./nets_engine");
        cppEngineAvailable = engine.exists() && engine.canExecute();
        if (!cppEngineAvailable) {
            System.out.println("C++ engine not found. Running in standalone mode.");
        }
    }

    public void initGame(int rows, int cols) {
        try {
            // Create new game state
            gameState = createNewGameState(rows, cols);
            gameBoard.loadGameState(gameState);
            setupEventHandlers();

            // Save initial state
            try {
                File jsonDir = new File(jsonFile).getParentFile();
                if (jsonDir != null && !jsonDir.exists()) {
                    jsonDir.mkdirs();
                }
                JsonHandler.saveGameState(gameState, jsonFile);
                System.out.println("Game state saved to: " + new File(jsonFile).getAbsolutePath());
            } catch (IOException e) {
                System.out.println("Could not save initial state: " + e.getMessage());
                // Continue anyway - game works in memory
            }
        } catch (Exception e) {
            showError("Failed to initialize game: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void loadExistingGame() {
        try {
            File file = new File(jsonFile);
            if (!file.exists()) {
                showError("No saved game found at: " + file.getAbsolutePath() +
                        "\n\nPlease start a new game instead.");
                return;
            }

            gameState = JsonHandler.loadGameState(jsonFile);
            gameBoard.loadGameState(gameState);
            setupEventHandlers();
            System.out.println("Game loaded from: " + file.getAbsolutePath());
        } catch (IOException e) {
            showError("Failed to load game: " + e.getMessage() +
                    "\n\nFile: " + new File(jsonFile).getAbsolutePath());
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
        meta.setSeed(0);
        meta.setWraps(false);
        state.setMeta(meta);

        // Initialize grid with random tiles
        Tile[][] grid = new Tile[rows][cols];
        Random random = new Random();
        TileType[] types = {TileType.STRAIGHT, TileType.CORNER, TileType.T_JUNCTION, TileType.CROSS};
        int[] rotations = {0, 90, 180, 270};

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                TileType type = types[random.nextInt(types.length)];
                int rotation = rotations[random.nextInt(rotations.length)];
                grid[i][j] = new Tile(type, rotation, false);
            }
        }

        // Add power source at random location
        int powerRow = random.nextInt(rows);
        int powerCol = random.nextInt(cols);
        grid[powerRow][powerCol] = new Tile(TileType.POWER, 0, true);

        // Add PC at different random location
        int pcRow, pcCol;
        do {
            pcRow = random.nextInt(rows);
            pcCol = random.nextInt(cols);
        } while (pcRow == powerRow && pcCol == powerCol);
        grid[pcRow][pcCol] = new Tile(TileType.PC, random.nextInt(4) * 90, false);

        state.setGrid(grid);

        // Initialize stats
        Stats stats = new Stats();
        stats.setComponents(calculateComponents(grid));
        stats.setLooseEnds(calculateLooseEnds(grid));
        stats.setSolved(false);
        state.setStats(stats);

        // Initialize rules
        Rules rules = new Rules();
        rules.setAllowLoops(false);
        Map<String, int[]> rotationRules = new HashMap<>();
        rotationRules.put("CORNER", new int[]{0, 90, 180, 270});
        rotationRules.put("STRAIGHT", new int[]{0, 90});
        rotationRules.put("T_JUNCTION", new int[]{0, 90, 180, 270});
        rotationRules.put("CROSS", new int[]{0});
        rotationRules.put("PC", new int[]{0, 90, 180, 270});
        rotationRules.put("POWER", new int[]{0});
        rotationRules.put("EMPTY", new int[]{});
        rules.setRotationRules(rotationRules);
        state.setRules(rules);

        return state;
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

            // Recalculate stats
            updateStats();

            // Update UI
            TileView tileView = gameBoard.getTileView(row, col);
            tileView.updateTile(tile);
            gameBoard.updateUI();

            // Check if solved
            if (checkWinCondition()) {
                gameState.getMeta().setStatus("SOLVED");
                gameBoard.updateUI();
                showWinMessage();
                return;
            }

            // Save state
            try {
                JsonHandler.saveGameState(gameState, jsonFile);
            } catch (IOException e) {
                System.out.println("Could not save state: " + e.getMessage());
            }

            // Switch to CPU turn
            gameState.getMeta().setTurn("CPU");
            gameBoard.updateUI();

            // Execute CPU turn (with or without C++ engine)
            executeCpuTurn();

        } catch (Exception e) {
            showError("Error processing move: " + e.getMessage());
        }
    }

    private void executeCpuTurn() {
        new Thread(() -> {
            try {
                Thread.sleep(500); // Small delay for visual effect

                if (cppEngineAvailable) {
                    // Use C++ engine
                    try {
                        JsonHandler.callCppEngine(jsonFile, jsonFile);
                        Platform.runLater(() -> {
                            try {
                                loadExistingGame();
                                checkAndHandleWin();
                            } catch (Exception e) {
                                System.out.println("C++ engine failed, using fallback: " + e.getMessage());
                                performStandaloneCpuMove();
                            }
                        });
                    } catch (Exception e) {
                        System.out.println("C++ engine error, using fallback: " + e.getMessage());
                        Platform.runLater(this::performStandaloneCpuMove);
                    }
                } else {
                    // Standalone CPU logic
                    Platform.runLater(this::performStandaloneCpuMove);
                }

            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }).start();
    }

    private void performStandaloneCpuMove() {
        // Simple CPU AI: find best move based on reducing loose ends
        List<int[]> possibleMoves = new ArrayList<>();
        Tile[][] grid = gameState.getGrid();

        for (int row = 0; row < grid.length; row++) {
            for (int col = 0; col < grid[row].length; col++) {
                if (!grid[row][col].isLocked()) {
                    possibleMoves.add(new int[]{row, col});
                }
            }
        }

        if (possibleMoves.isEmpty()) {
            gameState.getMeta().setTurn("HUMAN");
            gameBoard.updateUI();
            return;
        }

        // Evaluate each possible move
        int bestRow = -1, bestCol = -1, bestRotation = 0;
        int bestScore = Integer.MAX_VALUE;

        for (int[] move : possibleMoves) {
            int row = move[0];
            int col = move[1];
            Tile tile = grid[row][col];
            int originalRotation = tile.getRotation();

            // Try all possible rotations
            for (int rotation : new int[]{90, 180, 270}) {
                tile.setRotation((originalRotation + rotation) % 360);
                int score = calculateLooseEnds(grid);

                if (score < bestScore) {
                    bestScore = score;
                    bestRow = row;
                    bestCol = col;
                    bestRotation = (originalRotation + rotation) % 360;
                }
            }

            // Restore original rotation
            tile.setRotation(originalRotation);
        }

        // Apply best move
        if (bestRow != -1) {
            Tile tile = grid[bestRow][bestCol];
            tile.setRotation(bestRotation);

            Move move = new Move("CPU", bestRow, bestCol, bestRotation);
            gameState.setLastMove(move);

            // Update stats
            updateStats();

            // Update UI
            TileView tileView = gameBoard.getTileView(bestRow, bestCol);
            tileView.updateTile(tile);
        }

        // Switch back to human
        gameState.getMeta().setTurn("HUMAN");
        gameBoard.updateUI();

        // Save state
        try {
            JsonHandler.saveGameState(gameState, jsonFile);
        } catch (IOException e) {
            System.out.println("Could not save state: " + e.getMessage());
        }

        checkAndHandleWin();
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
                    dfs(grid, visited, i, j);
                    components++;
                }
            }
        }

        return components;
    }

    private void dfs(Tile[][] grid, boolean[][] visited, int i, int j) {
        if (i < 0 || i >= grid.length || j < 0 || j >= grid[0].length) return;
        if (visited[i][j] || grid[i][j].getType() == TileType.EMPTY) return;

        visited[i][j] = true;
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
                        dfs(grid, visited, ni, nj);
                    }
                }
            }
        }
    }

    private boolean[] getConnections(Tile tile) {
        // Returns [top, right, bottom, left]
        boolean[] conn = new boolean[4];
        int rot = tile.getRotation();

        switch (tile.getType()) {
            case STRAIGHT:
                conn[0] = conn[2] = (rot == 0);
                conn[1] = conn[3] = (rot == 90);
                break;
            case CORNER:
                if (rot == 0) { conn[0] = conn[1] = true; }
                else if (rot == 90) { conn[1] = conn[2] = true; }
                else if (rot == 180) { conn[2] = conn[3] = true; }
                else { conn[3] = conn[0] = true; }
                break;
            case T_JUNCTION:
                if (rot == 0) { conn[0] = conn[1] = conn[3] = true; }
                else if (rot == 90) { conn[0] = conn[1] = conn[2] = true; }
                else if (rot == 180) { conn[1] = conn[2] = conn[3] = true; }
                else { conn[0] = conn[2] = conn[3] = true; }
                break;
            case CROSS:
                conn[0] = conn[1] = conn[2] = conn[3] = true;
                break;
            case PC:
            case POWER:
                conn[0] = conn[2] = (rot % 180 == 0);
                conn[1] = conn[3] = (rot % 180 == 90);
                break;
        }

        return conn;
    }

    private boolean checkWinCondition() {
        Stats stats = gameState.getStats();
        return stats.getLooseEnds() == 0 && stats.getComponents() == 1;
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
        alert.setContentText("You've successfully solved the puzzle!\n\nAll tiles are connected in a single network.");
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