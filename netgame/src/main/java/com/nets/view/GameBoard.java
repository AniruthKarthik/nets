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

public class GameBoard extends VBox {
    private GridPane gridPane;
    private TileView[][] tileViews;
    private Label statusLabel;
    private Label statsLabel;
    private GameState gameState;

    public GameBoard() {
        setSpacing(20);
        setPadding(new Insets(20));
        setAlignment(Pos.CENTER);
        setStyle("-fx-background-color: #1a1a2e;");

        // Status label
        statusLabel = new Label();
        statusLabel.setFont(Font.font("Arial", FontWeight.BOLD, 24));
        statusLabel.setTextFill(Color.WHITE);

        // Grid pane
        gridPane = new GridPane();
        gridPane.setHgap(2);
        gridPane.setVgap(2);
        gridPane.setAlignment(Pos.CENTER);
        gridPane.setStyle("-fx-background-color: #16213e;");
        gridPane.setPadding(new Insets(10));

        // Stats label
        statsLabel = new Label();
        statsLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
        statsLabel.setTextFill(Color.rgb(200, 200, 200));

        getChildren().addAll(statusLabel, gridPane, statsLabel);
    }

    public void loadGameState(GameState state) {
        this.gameState = state;
        gridPane.getChildren().clear();

        int height = state.getMeta().getHeight();
        int width = state.getMeta().getWidth();
        tileViews = new TileView[height][width];

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                Tile tile = state.getGrid()[row][col];
                TileView tileView = new TileView(tile, row, col);
                tileViews[row][col] = tileView;
                gridPane.add(tileView, col, row);
            }
        }

        updateUI();
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