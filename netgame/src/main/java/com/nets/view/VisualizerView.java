package com.nets.view;

import com.nets.model.*;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Separator;
import javafx.scene.control.Slider;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.util.Duration;
import javafx.scene.shape.Rectangle;

import java.util.List;

public class VisualizerView extends VBox {
    private GridPane gridPane;
    private TileView[][] tileViews;
    private List<VisualStep> steps;
    private int[][] preMoveRotations;
    private Move actualMove;
    private GameState initialState;
    
    private Label statusLabel;
    private Label statsLabel;
    private Label stepInfoLabel;
    private Label scoreLabel;
    private Label algoLabel;
    private Slider speedSlider;
    
    private boolean playing = false;
    private Runnable onClose;
    private String currentAlgoDisplay;
    private HBox middleContainer;
    private Timeline highlightTimeline;
    private Rectangle selectionFrame;

    public VisualizerView(GameState state, List<VisualStep> steps, int[][] preMoveRotations, Move actualMove, String algoDisplayName, Scene scene, Runnable onClose) {
        this.initialState = state;
        this.steps = steps;
        this.preMoveRotations = preMoveRotations;
        this.actualMove = actualMove;
        this.currentAlgoDisplay = algoDisplayName;
        this.onClose = onClose;

        setSpacing(20);
        setPadding(new Insets(20));
        setAlignment(Pos.CENTER);
        setStyle("-fx-background-color: #1a1a2e;");
        
        String css = getClass().getResource("/com/nets/styles.css").toExternalForm();
        this.getStylesheets().add(css);

        statusLabel = new Label("AI VISUALIZER: " + algoDisplayName.toUpperCase());
        statusLabel.styleProperty().bind(scene.widthProperty().divide(45).asString("-fx-text-fill: #ff9664; -fx-font-size: %fpx; -fx-font-weight: bold;"));

        middleContainer = new HBox();
        middleContainer.setAlignment(Pos.CENTER);
        middleContainer.spacingProperty().bind(scene.widthProperty().multiply(0.05));

        VBox gridBox = new VBox();
        gridBox.setAlignment(Pos.CENTER);
        setupGrid();
        gridBox.getChildren().add(gridPane);
        
        middleContainer.getChildren().addAll(gridBox, createSidePanel(scene));

        statsLabel = new Label("Evaluation steps: " + (steps != null ? steps.size() : 0));
        statsLabel.styleProperty().bind(scene.widthProperty().divide(65).asString("-fx-text-fill: #aaa; -fx-font-size: %fpx;"));

        HBox controls = createControls(scene);

        getChildren().addAll(statusLabel, middleContainer, statsLabel, controls);

        Platform.runLater(this::updateTileSizes);
        widthProperty().addListener((obs, oldVal, newVal) -> updateTileSizes());
        heightProperty().addListener((obs, oldVal, newVal) -> updateTileSizes());
    }

    private void setupGrid() {
        gridPane = new GridPane();
        gridPane.setHgap(2);
        gridPane.setVgap(2);
        gridPane.setAlignment(Pos.CENTER);
        gridPane.setStyle("-fx-background-color: #16213e;");
        gridPane.setPadding(new Insets(10));

        int rows = initialState.getMeta().getHeight();
        int cols = initialState.getMeta().getWidth();
        tileViews = new TileView[rows][cols];

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                Tile original = initialState.getGrid()[r][c];
                int rot = (preMoveRotations != null) ? preMoveRotations[r][c] : original.getRotation();
                Tile copy = new Tile(original.getType(), rot, original.isLocked());
                copy.setConnections(original.getConnections());
                TileView tv = new TileView(copy, r, c, 50); 
                tileViews[r][c] = tv;
                gridPane.add(tv, c, r);
            }
        }

        // Dedicated neon highlight frame - UNMANAGED to not affect grid layout
        selectionFrame = new Rectangle();
        selectionFrame.setFill(Color.rgb(255, 255, 0, 0.15));
        selectionFrame.setStroke(Color.rgb(255, 255, 0, 0.9));
        selectionFrame.setStrokeWidth(5);
        selectionFrame.setArcWidth(10);
        selectionFrame.setArcHeight(10);
        selectionFrame.setVisible(false);
        selectionFrame.setMouseTransparent(true);
        selectionFrame.setManaged(false); // CRITICAL: This prevents the gap issue
        gridPane.getChildren().add(selectionFrame); 
    }

    private void updateTileSizes() {
        if (initialState == null || tileViews == null) return;
        double availableWidth = getWidth() * 0.7; 
        double availableHeight = getHeight() * 0.7;
        if (availableWidth <= 0) availableWidth = 600;
        if (availableHeight <= 0) availableHeight = 600;

        double sizeW = availableWidth / initialState.getMeta().getWidth();
        double sizeH = availableHeight / initialState.getMeta().getHeight();
        double tileSize = Math.min(sizeW, sizeH);
        tileSize = Math.max(10, tileSize);

        for (int r = 0; r < tileViews.length; r++) {
            for (int c = 0; c < tileViews[r].length; c++) {
                tileViews[r][c].setSize(tileSize);
            }
        }

        // Sized slightly larger than the tile
        selectionFrame.setWidth(tileSize + 8);
        selectionFrame.setHeight(tileSize + 8);
    }

    private VBox createSidePanel(Scene scene) {
        VBox sidePanel = new VBox();
        sidePanel.setAlignment(Pos.CENTER);
        sidePanel.prefWidthProperty().bind(scene.widthProperty().multiply(0.18));
        sidePanel.spacingProperty().bind(scene.heightProperty().multiply(0.02));

        Label titleLabel = new Label("STEP DETAILS");
        titleLabel.styleProperty().bind(scene.widthProperty().divide(60).asString("-fx-text-fill: #00d4ff; -fx-font-size: %fpx; -fx-font-weight: bold;"));

        VBox detailsBox = new VBox();
        detailsBox.setAlignment(Pos.CENTER_LEFT);
        detailsBox.paddingProperty().bind(javafx.beans.binding.Bindings.createObjectBinding(() -> new Insets(scene.getWidth() * 0.012), scene.widthProperty()));
        detailsBox.setStyle("-fx-background-color: #16213e; -fx-background-radius: 12; -fx-border-color: #0f3460; -fx-border-radius: 12; -fx-border-width: 2;");
        detailsBox.spacingProperty().bind(scene.heightProperty().multiply(0.02));

        algoLabel = new Label("Algo: " + currentAlgoDisplay);
        algoLabel.styleProperty().bind(scene.widthProperty().divide(80).asString("-fx-text-fill: white; -fx-font-size: %fpx; -fx-font-weight: bold;"));

        stepInfoLabel = new Label("Ready to replay...");
        stepInfoLabel.styleProperty().bind(scene.widthProperty().divide(90).asString("-fx-text-fill: #00d4ff; -fx-font-size: %fpx;"));
        stepInfoLabel.setWrapText(true);

        scoreLabel = new Label("Score: ---");
        scoreLabel.styleProperty().bind(scene.widthProperty().divide(70).asString("-fx-text-fill: #ffc107; -fx-font-size: %fpx; -fx-font-weight: bold;"));

        VBox speedBox = new VBox(8);
        speedBox.setAlignment(Pos.CENTER);
        Label speedLabel = new Label("Visualizer Speed (0-500):");
        speedLabel.styleProperty().bind(scene.widthProperty().divide(100).asString("-fx-text-fill: white; -fx-font-size: %fpx;"));
        speedSlider = new Slider(0, 500, 250);
        speedSlider.setPrefWidth(180);
        speedBox.getChildren().addAll(speedLabel, speedSlider);

        detailsBox.getChildren().addAll(algoLabel, new Separator(), stepInfoLabel, scoreLabel, new Separator(), speedBox);
        sidePanel.getChildren().addAll(titleLabel, detailsBox);
        return sidePanel;
    }

    private HBox createControls(Scene scene) {
        HBox controls = new HBox();
        controls.setAlignment(Pos.CENTER);
        controls.spacingProperty().bind(scene.widthProperty().multiply(0.03));
        String baseStyle = "-fx-text-fill: white; -fx-font-weight: bold; -fx-cursor: hand; -fx-background-radius: 8;";
        javafx.beans.binding.StringBinding btnStyle = scene.widthProperty().divide(110).asString(baseStyle + "-fx-font-size: %fpx; -fx-padding: 8 16;");

        Button playBtn = new Button("Play Visualization");
        playBtn.styleProperty().bind(javafx.beans.binding.Bindings.concat(btnStyle, "-fx-background-color: #00d4ff;"));
        playBtn.setOnAction(e -> startVisualization());

        Button closeBtn = new Button("Close Visualizer");
        closeBtn.styleProperty().bind(javafx.beans.binding.Bindings.concat(btnStyle, "-fx-background-color: #e94560;"));
        closeBtn.setOnAction(e -> { 
            playing = false; 
            if (highlightTimeline != null) highlightTimeline.stop();
            onClose.run(); 
        });

        controls.getChildren().addAll(playBtn, closeBtn);
        return controls;
    }

    private void startVisualization() {
        if (playing || steps == null) return;
        playing = true;
        if (highlightTimeline != null) highlightTimeline.stop();
        selectionFrame.setVisible(false);
        
        for (int r = 0; r < tileViews.length; r++) {
            for (int c = 0; c < tileViews[r].length; c++) {
                tileViews[r][c].getTile().setRotation(preMoveRotations[r][c]);
                tileViews[r][c].setOpacity(1.0);
                tileViews[r][c].setStyle("");
                tileViews[r][c].draw();
            }
        }

        new Thread(() -> {
            for (int i = 0; i < steps.size(); i++) {
                if (!playing) break;
                final int idx = i;
                VisualStep step = steps.get(idx);
                
                Platform.runLater(() -> {
                    int r = step.getRow();
                    int c = step.getCol();
                    TileView tv = tileViews[r][c];
                    if ("SCORE".equals(step.getType())) {
                        scoreLabel.setText(String.format("Score: %.2f", step.getScore()));
                        tv.setStyle("-fx-border-color: #00d4ff; -fx-border-width: 4; -fx-background-color: rgba(0, 212, 255, 0.2);");
                    } else {
                        tv.getTile().setRotation(step.getRotation());
                        tv.draw();
                        tv.setStyle("-fx-border-color: #00d4ff; -fx-border-width: 2;");
                        stepInfoLabel.setText(String.format("Search: %s (%d, %d)", step.getType(), r, c));
                    }
                });

                try {
                    double val = speedSlider.getValue();
                    long delay = (long) (1500.0 * Math.pow(0.001, val / 500.0));
                    Thread.sleep(Math.max(2, delay));
                } catch (InterruptedException e) { break; }

                Platform.runLater(() -> {
                    tileViews[step.getRow()][step.getCol()].setStyle("");
                });
            }

            if (playing && actualMove != null) {
                try { Thread.sleep(300); } catch (InterruptedException e) {}
                Platform.runLater(() -> {
                    int r = actualMove.getRow();
                    int c = actualMove.getCol();
                    TileView tv = tileViews[r][c];
                    tv.getTile().setRotation(actualMove.getRotation());
                    tv.draw();
                    
                    showFinalSelectionFrame(r, c);
                    
                    stepInfoLabel.setText("FINAL MOVE EXECUTED");
                    statusLabel.setText("VISUALIZATION COMPLETE");
                    scoreLabel.setText("Move at (" + r + "," + c + ")");
                });
            }
            playing = false;
        }).start();
    }

    private void showFinalSelectionFrame(int row, int col) {
        selectionFrame.setVisible(true);
        selectionFrame.toFront();
        
        // Manual positioning based on grid layout to avoid 'pushing' columns
        double tileSize = tileViews[0][0].getWidth();
        double hgap = gridPane.getHgap();
        double vgap = gridPane.getVgap();
        Insets padding = gridPane.getPadding();
        
        // Calculate X and Y including padding and gaps
        double x = padding.getLeft() + col * (tileSize + hgap) - 4; // -4 to center the +8 width
        double y = padding.getTop() + row * (tileSize + vgap) - 4;
        
        selectionFrame.setLayoutX(x);
        selectionFrame.setLayoutY(y);
        
        highlightTimeline = new Timeline(
            new KeyFrame(Duration.ZERO, 
                new KeyValue(selectionFrame.opacityProperty(), 0.3),
                new KeyValue(selectionFrame.scaleXProperty(), 0.9),
                new KeyValue(selectionFrame.scaleYProperty(), 0.9)),
            new KeyFrame(Duration.millis(400), 
                new KeyValue(selectionFrame.opacityProperty(), 1.0),
                new KeyValue(selectionFrame.scaleXProperty(), 1.15),
                new KeyValue(selectionFrame.scaleYProperty(), 1.15)),
            new KeyFrame(Duration.millis(800), 
                new KeyValue(selectionFrame.opacityProperty(), 0.3),
                new KeyValue(selectionFrame.scaleXProperty(), 0.9),
                new KeyValue(selectionFrame.scaleYProperty(), 0.9))
        );
        highlightTimeline.setCycleCount(Timeline.INDEFINITE);
        highlightTimeline.play();
    }
}
