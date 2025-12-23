package com.nets.controller;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.nets.model.*;
import com.nets.view.GameBoard;
import com.nets.view.TileView;
import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.input.MouseButton;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.CompletableFuture;

public class GameController {
    private GameBoard gameBoard;
    private GameState gameState;
    private final Gson gson;
    private static final String JSON_FILE = "../json/board_state.jsonc";
    private static final String ENGINE_PATH = "../nets_engine";

    public GameController(GameBoard gameBoard) {
        this.gameBoard = gameBoard;
        this.gson = new GsonBuilder().setPrettyPrinting().create();
    }

    public void initGame(int rows, int cols) {
        try {
            // 1. Generate initial state in Java (as requested, keeping generation in Java)
            gameState = createNewGameState(rows, cols);
            
            // 2. Save state to JSON
            saveGameState(gameState);

            // 3. Run Engine (to get initial stats and powered status)
            runEngine();

            // 4. Load state back
            gameState = loadGameState();

            // 5. Update UI
            gameBoard.loadGameState(gameState);
            setupEventHandlers();

        } catch (Exception e) {
            showError("Failed to initialize game: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private void saveGameState(GameState state) throws IOException {
        try (Writer writer = new FileWriter(JSON_FILE)) {
            gson.toJson(state, writer);
        }
    }

    private GameState loadGameState() throws IOException {
        // Read the entire file content into a String first to handle potential closing issues
        String content = new String(Files.readAllBytes(Paths.get(JSON_FILE)));
        return gson.fromJson(content, GameState.class);
    }

    private void runEngine() throws IOException, InterruptedException {
        ProcessBuilder pb = new ProcessBuilder(ENGINE_PATH, JSON_FILE, JSON_FILE);
        // Ensure the engine runs in the correct directory (root of the project, or consistent with relative paths)
        // Java runs in netgame/ so ../ is root.
        // We can set directory to root.
        pb.directory(new File("..")); // Set working directory to project root
        
        // Wait, if I set directory to "..", then "nets_engine" (command) should be "./nets_engine"
        // and arguments "json/board_state.jsonc".
        // Let's adjust.
        pb.command("./nets_engine", "json/board_state.jsonc", "json/board_state.jsonc");
        
        Process process = pb.start();
        int exitCode = process.waitFor();
        
        if (exitCode != 0) {
            // Read error stream
            String error = new String(process.getErrorStream().readAllBytes());
            throw new IOException("Engine failed with code " + exitCode + ": " + error);
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
        TileType[] types = {TileType.STRAIGHT, TileType.CORNER, TileType.T_JUNCTION};
        int[] rotations = {0, 90, 180, 270};

        // Fill grid with random wire tiles
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

        // Add multiple PCs (25-40% of board should be PCs)
        int totalTiles = rows * cols;
        int numPCs = Math.max(3, (int)(totalTiles * 0.3)); // At least 3 PCs, or 30% of tiles

        Set<String> occupiedPositions = new HashSet<>();
        occupiedPositions.add(powerRow + "," + powerCol); // Power position is occupied

        int pcsAdded = 0;
        int attempts = 0;
        while (pcsAdded < numPCs && attempts < totalTiles * 2) {
            int pcRow = random.nextInt(rows);
            int pcCol = random.nextInt(cols);
            String position = pcRow + "," + pcCol;

            if (!occupiedPositions.contains(position)) {
                grid[pcRow][pcCol] = new Tile(TileType.PC, random.nextInt(4) * 90, false);
                occupiedPositions.add(position);
                pcsAdded++;
            }
            attempts++;
        }

        state.setGrid(grid);

        // Initialize stats (dummy values, engine will calculate real ones)
        Stats stats = new Stats();
        stats.setComponents(0);
        stats.setLooseEnds(0);
        stats.setSolved(false);
        state.setStats(stats);

        // Initialize rules
        Rules rules = new Rules();
        rules.setAllowLoops(false);
        Map<String, int[]> rotationRules = new HashMap<>();
        rotationRules.put("CORNER", new int[]{0, 90, 180, 270});
        rotationRules.put("STRAIGHT", new int[]{0, 90});
        rotationRules.put("T_JUNCTION", new int[]{0, 90, 180, 270});
        rotationRules.put("PC", new int[]{0, 90, 180, 270});
        rotationRules.put("POWER", new int[]{0});
        rotationRules.put("EMPTY", new int[]{});
        rules.setRotationRules(rotationRules);
        state.setRules(rules);
        
        // Initialize Last Move
        state.setLastMove(new Move("HUMAN", -1, -1, 0));

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
            // 1. Update local state
            Tile tile = gameState.getGrid()[row][col];
            tile.rotate(rotation);

            // 2. Set Move Info
            Move move = new Move("HUMAN", row, col, tile.getRotation());
            gameState.setLastMove(move);
            gameState.getMeta().setTurn("HUMAN"); // Ensure turn is HUMAN

            // 3. Save, Run Engine (Analyze), Load
            saveGameState(gameState);
            runEngine();
            gameState = loadGameState();

            // 4. Update UI with new stats/powered status
            updateUI();

            // 5. Check Win
            if (gameState.getStats().isSolved()) {
                handleWin();
                return;
            }

            // 6. CPU Turn
            // Trigger CPU turn with delay
            gameState.getMeta().setTurn("CPU");
            updateUI(); // Show that it's CPU turn (if UI supports it) or just wait

            CompletableFuture.runAsync(() -> {
                try {
                    Thread.sleep(500); // Visual delay
                    
                    // Save (with Turn=CPU)
                    saveGameState(gameState);
                    
                    // Run Engine (Executes CPU Move)
                    runEngine();
                    
                    Platform.runLater(() -> {
                        try {
                            // Load result
                            gameState = loadGameState();
                            updateUI();
                            
                            if (gameState.getStats().isSolved()) {
                                handleWin();
                            } else {
                                // Ensure turn is back to HUMAN (Engine should set it, but safety check)
                                if (!gameState.getStats().isSolved()) {
                                    gameState.getMeta().setTurn("HUMAN");
                                    // Actually engine sets it to HUMAN after move.
                                }
                            }
                        } catch (IOException e) {
                            showError("Error loading CPU move: " + e.getMessage());
                        }
                    });
                    
                } catch (Exception e) {
                    Platform.runLater(() -> showError("Error during CPU turn: " + e.getMessage()));
                }
            });

        } catch (Exception e) {
            showError("Error processing move: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private void updateUI() {
        for (int i = 0; i < gameState.getMeta().getHeight(); i++) {
            for (int j = 0; j < gameState.getMeta().getWidth(); j++) {
                gameBoard.getTileView(i, j).updateTile(gameState.getGrid()[i][j]);
            }
        }
        gameBoard.updateUI();
    }

    private void handleWin() {
        gameState.getMeta().setStatus("SOLVED");
        updateUI();
        showWinMessage();
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
