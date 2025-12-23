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
import java.util.Arrays;
import java.util.function.Consumer;

public class GameController {

    private String jsonFile;
    private GameBoard gameBoard;
    private GameState gameState;
    private final Consumer<GameState> onStateUpdate;


    public GameController(GameBoard gameBoard, Consumer<GameState> onStateUpdate) {
        this.gameBoard = gameBoard;
        this.onStateUpdate = onStateUpdate;
        setupJsonPath();
    }

    public String getJsonFile() {
        return jsonFile;
    }

    private void setupJsonPath() {
        // Define a single, reliable path for the JSON file.
        // The C++ engine and Java UI MUST share this exact path.
        String homeDir = System.getProperty("user.dir");
        jsonFile = new File(homeDir, "game_state.json").getAbsolutePath();
        System.out.println("Using shared JSON path: " + jsonFile);

        // Ensure the directory exists
        File jsonDir = new File(jsonFile).getParentFile();
        if (!jsonDir.exists()) {
            if (jsonDir.mkdirs()) {
                System.out.println("Created JSON directory: " + jsonDir.getPath());
            }
        }
    }

    public void initGame(int rows, int cols) {
        System.out.println("Requesting new game from engine: " + rows + "x" + cols);

        // Create a temporary 'command' state to request a new game
        this.gameState = new GameState();
        Meta meta = new Meta();
        meta.setStatus("NEW_GAME");
        meta.setWidth(cols);
        meta.setHeight(rows);
        this.gameState.setMeta(meta);

        // Save the command and run the engine
        saveStateAndNotify("Failed to generate new game board.");
    }

    public void loadExistingGame() {
        System.out.println("Loading existing game from: " + jsonFile);
        File file = new File(jsonFile);
        if (!file.exists()) {
            showError("No saved game found at: " + file.getAbsolutePath() +
                    "\n\nPlease start a new game instead.");
            return;
        }
        loadStateAndRefreshUI();
    }

    public void resetGame(int rows, int cols) {
        initGame(rows, cols);
    }

    private void handleHumanMove(int row, int col, MouseButton button) {
        if (this.gameState == null || !"HUMAN".equals(this.gameState.getMeta().getTurn())) {
            return; // Ignore clicks if it's not the human's turn
        }

        Tile tile = gameState.getGrid()[row][col];
        if (tile.isLocked()) {
            return; // Ignore clicks on locked on tiles
        }

        if (button == MouseButton.SECONDARY) {
            // Right-click to toggle lock
            tile.setLocked(!tile.isLocked());
            gameBoard.getTileView(row, col).updateTile(tile);
            saveStateAndNotify("Failed to save lock state.");
            return;
        }


        int currentRotation = tile.getRotation();
        int newRotation = (currentRotation + 90) % 360;


        System.out.println("Human move: Tile at [" + row + "," + col + "] rotated to " + newRotation);

        // Set the turn and last move, then save to JSON for the engine
        gameState.getMeta().setTurn("CPU");
        gameState.setLastMove(new Move("HUMAN", row, col, newRotation));

        // Immediately update the local tile for visual feedback before engine runs
        tile.setRotation(newRotation);
        gameBoard.getTileView(row, col).updateTile(tile);

        saveStateAndNotify("Error processing move.");
    }

    private void saveStateAndNotify(String errorMessage) {
        // Run on a background thread to keep UI responsive
        new Thread(() -> {
            try {
                // 1. Save the current state (which includes the new move/command)
                JsonHandler.saveGameState(this.gameState, this.jsonFile);

            } catch (IOException e) {
                Platform.runLater(() -> showError(errorMessage + "\n" + e.getMessage()));
                // Revert turn so player can try again
                if (this.gameState != null && this.gameState.getMeta() != null) {
                    this.gameState.getMeta().setTurn("HUMAN");
                    Platform.runLater(this::updateUI);
                }
                e.printStackTrace();
            }
        }).start();
    }

    private void loadStateAndRefreshUI() {
        try {
            this.gameState = JsonHandler.loadGameState(jsonFile);
            if (this.gameState == null) {
                showError("Failed to load game state from JSON. The file might be corrupted or empty.");
                return;
            }
            gameBoard.loadGameState(this.gameState);
            setupEventHandlers(); // Re-attach handlers to the new TileView objects
            updateUI();
        } catch (IOException e) {
            showError("Failed to read game state from JSON after engine run.\n" + e.getMessage());
            e.printStackTrace();
        }
    }
    private void updateUI() {
        gameBoard.updateUI();
        if (onStateUpdate != null && gameState != null) {
            onStateUpdate.accept(gameState);
        }
    }
    private void setupEventHandlers() {
        TileView[][] tileViews = gameBoard.getTileViews();
        if (tileViews == null) return;

        for (int row = 0; row < tileViews.length; row++) {
            for (int col = 0; col < tileViews[row].length; col++) {
                TileView tileView = tileViews[row][col];
                if (tileView == null) continue;

                // Remove any existing handlers to prevent duplicates
                tileView.setOnMouseClicked(null);
                tileView.setOnMouseEntered(null);
                tileView.setOnMouseExited(null);

                // Add new handlers
                tileView.setOnMouseClicked(event -> {
                    if (gameState.getMeta() != null && "HUMAN".equals(gameState.getMeta().getTurn())) {
                        handleHumanMove(tileView.getRow(), tileView.getCol(), event.getButton());
                    }
                });

                tileView.setOnMouseEntered(event -> {
                    if (gameState.getMeta() != null && "HUMAN".equals(gameState.getMeta().getTurn()) && !tileView.getTile().isLocked()) {
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

    private void showError(String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Error");
        alert.setHeaderText("An unexpected error occurred.");
        alert.setContentText(message);
        alert.showAndWait();
    }
}