package com.nets.view;

import com.nets.model.GameState;
import com.nets.model.Stats;
import com.nets.model.Tile;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.control.Label;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

import javafx.stage.Screen;
import javafx.geometry.Rectangle2D;

public class GameBoard extends VBox {
    private GridPane gridPane;
    private TileView[][] tileViews;
    private Label statusLabel;
    private Label statsLabel;
    private GameState gameState;
    private javafx.scene.layout.HBox middleContainer;

    public GameBoard() {
        setSpacing(20);
        setPadding(new Insets(20));
        setAlignment(Pos.CENTER);
        setStyle("-fx-background-color: #1a1a2e;");

        // Status label
        statusLabel = new Label();
        statusLabel.setTextFill(Color.WHITE);

        // Middle container for grid and potential side panel
        middleContainer = new javafx.scene.layout.HBox(80);
        middleContainer.setAlignment(Pos.CENTER);

        // Grid pane
        gridPane = new GridPane();
        gridPane.setHgap(2);
        gridPane.setVgap(2);
        gridPane.setAlignment(Pos.CENTER);
        gridPane.setStyle("-fx-background-color: #16213e;");
        gridPane.setPadding(new Insets(10));
        
        middleContainer.getChildren().add(gridPane);

        // Stats label
        statsLabel = new Label();
        statsLabel.setTextFill(Color.rgb(200, 200, 200));

        getChildren().addAll(statusLabel, middleContainer, statsLabel);

        // Responsive font sizes and tile resizing
        widthProperty().addListener((obs, oldVal, newVal) -> updateResponsiveSizes());
        heightProperty().addListener((obs, oldVal, newVal) -> updateResponsiveSizes());
    }

    public void updateResponsiveSizes() {
        double w = getWidth();
        double h = getHeight();
        if (w <= 0 || h <= 0) return;

        // Relative font sizes - toned down for neatness
        double baseSize = Math.min(w, h);
        statusLabel.setFont(Font.font("Arial", FontWeight.BOLD, baseSize * 0.035));
        statsLabel.setFont(Font.font("Arial", FontWeight.NORMAL, baseSize * 0.02));

        // Resize tiles
        if (gameState != null && tileViews != null) {
            double tileSize = calculateTileSize(gameState.getMeta().getWidth(), gameState.getMeta().getHeight());
            for (int row = 0; row < tileViews.length; row++) {
                for (int col = 0; col < tileViews[row].length; col++) {
                    if (tileViews[row][col] != null) {
                        tileViews[row][col].setSize(tileSize);
                    }
                }
            }
        }
    }

    public void setSidePanel(javafx.scene.Node node) {
        if (middleContainer.getChildren().size() > 1) {
            middleContainer.getChildren().remove(1);
        }
        if (node != null) {
            middleContainer.getChildren().add(node);
        }
    }

    public double calculateTileSize(int width, int height) {
        // Use container size for calculation instead of screen bounds
        double availableWidth = getWidth() * 0.7; // Reserve space for side panel
        double availableHeight = getHeight() * 0.7;
        
        if (availableWidth <= 0) {
             javafx.stage.Screen screen = javafx.stage.Screen.getPrimary();
             availableWidth = screen.getVisualBounds().getWidth() * 0.5;
        }
        if (availableHeight <= 0) {
             javafx.stage.Screen screen = javafx.stage.Screen.getPrimary();
             availableHeight = screen.getVisualBounds().getHeight() * 0.5;
        }

        double sizeW = availableWidth / width;
        double sizeH = availableHeight / height;

        double size = Math.min(sizeW, sizeH);
        return Math.max(10, size); 
    }

    public void loadGameState(GameState state) {
        this.gameState = state;
        gridPane.getChildren().clear();

        int height = state.getMeta().getHeight();
        int width = state.getMeta().getWidth();
        tileViews = new TileView[height][width];

        double tileSize = calculateTileSize(width, height);

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                Tile tile = state.getGrid()[row][col];
                TileView tileView = new TileView(tile, row, col, tileSize);
                tileViews[row][col] = tileView;
                gridPane.add(tileView, col, row);
            }
        }

        updateUI();
        updateResponsiveSizes();
    }

    public void updateUI() {
        if (gameState == null) return;

        // Update status
        String turn = gameState.getMeta().getTurn();
        String status = gameState.getMeta().getStatus();

        if (status.equals("SOLVED")) {
            statusLabel.setText("🎉 PUZZLE SOLVED! 🎉");
            statusLabel.setTextFill(Color.rgb(0, 255, 100));
        } else {
            statusLabel.setText("Turn: " + turn);
            statusLabel.setTextFill(turn.equals("HUMAN") ? Color.rgb(100, 200, 255) : Color.rgb(255, 150, 100));
        }

        // Update stats
        Stats stats = gameState.getStats();
        statsLabel.setText(String.format(
                "Components: %d | Loose Ends: %d | %s",
                stats.getComponents(),
                stats.getLooseEnds(),
                stats.isSolved() ? "SOLVED ✓" : "Not Solved"
        ));
    }

    public TileView getTileView(int row, int col) {
        return tileViews[row][col];
    }

    public TileView[][] getTileViews() {
        return tileViews;
    }

    public GameState getGameState() {
        return gameState;
    }

    public void rotateBoardVisual() {
        gridPane.setRotate(gridPane.getRotate() + 90);
    }
}