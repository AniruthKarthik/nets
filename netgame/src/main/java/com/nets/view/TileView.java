package com.nets.view;

import com.nets.model.Tile;
import com.nets.model.TileType;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.util.Duration;

public class TileView extends Canvas {
    private double size;
    private double lineWidth;
    private double currentVisualRotation;
    private Timeline rotationTimeline;

    // Wire colors
    private static final Color WIRE_POWERED_COLOR = Color.rgb(0, 255, 100); // Bright Green - for connected wires
    private static final Color WIRE_UNPOWERED_COLOR = Color.rgb(255, 200, 0); // Gold/Yellow - for disconnected wires

    // PC colors
    private static final Color PC_POWERED_COLOR = Color.rgb(0, 200, 255); // Cyan/Light Blue - for connected PCs
    private static final Color PC_UNPOWERED_COLOR = Color.rgb(150, 150, 150); // Gray - for disconnected PCs

    private static final Color BG_COLOR = Color.rgb(40, 40, 60);
    private static final Color LOCKED_COLOR = Color.rgb(60, 60, 80);

    private Tile tile;
    private int row;
    private int col;

    public TileView(Tile tile, int row, int col, double size) {
        super(size, size);
        this.size = size;
        this.lineWidth = size * 0.075; // Proportional line width
        this.tile = tile;
        this.row = row;
        this.col = col;
        this.currentVisualRotation = tile.getRotation();
        draw();
    }

    public void setSize(double size) {
        this.size = size;
        this.lineWidth = size * 0.075;
        setWidth(size);
        setHeight(size);
        draw();
    }

    public void setRotationAnimated(int targetRotation, double durationMs, boolean clockwise) {
        if (rotationTimeline != null) {
            rotationTimeline.stop();
        }

        // Sync property value to current state before starting new animation
        currentVisualRotationProperty().set(currentVisualRotation);

        double startRot = currentVisualRotation;
        
        // Normalize target to be 0-359
        double normalizedTarget = targetRotation % 360;
        if (normalizedTarget < 0) normalizedTarget += 360;
        final double finalTarget = normalizedTarget;
        
        // Normalize start to 0-359 for calculation
        double startNormalized = startRot % 360;
        if (startNormalized < 0) startNormalized += 360;

        // Calculate difference based on direction
        double diff = finalTarget - startNormalized;
        if (clockwise) {
            if (diff < 0) diff += 360;
        } else {
            if (diff > 0) diff -= 360;
        }
        
        // If diff is 0, we're already there
        if (diff == 0 && durationMs > 0) {
            return;
        }

        double endRot = startRot + diff;

        if (durationMs <= 0) {
            currentVisualRotationProperty().set(finalTarget);
            return;
        }

        rotationTimeline = new Timeline(
            new KeyFrame(Duration.millis(durationMs),
                new KeyValue(currentVisualRotationProperty(), endRot)
            )
        );
        rotationTimeline.setOnFinished(e -> {
            // Reset to normalized range for stability, but use property to keep in sync
            currentVisualRotationProperty().set(finalTarget);
        });
        rotationTimeline.play();
    }

    private javafx.beans.property.DoubleProperty currentVisualRotationProperty;
    private javafx.beans.property.DoubleProperty currentVisualRotationProperty() {
        if (currentVisualRotationProperty == null) {
            currentVisualRotationProperty = new javafx.beans.property.SimpleDoubleProperty(currentVisualRotation) {
                @Override
                protected void invalidated() {
                    currentVisualRotation = get();
                    draw();
                }
            };
        }
        return currentVisualRotationProperty;
    }

    public void draw() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0, 0, size, size);

        // Background
        gc.setFill(tile.isLocked() ? LOCKED_COLOR : BG_COLOR);
        gc.fillRect(0, 0, size, size);

        // Border
        gc.setStroke(Color.rgb(80, 80, 100));
        gc.setLineWidth(1);
        gc.strokeRect(0, 0, size, size);

        // Draw tile based on type and rotation
        gc.save();
        gc.translate(size / 2, size / 2);
        gc.rotate(currentVisualRotation);
        gc.translate(-size / 2, -size / 2);

        // Choose color based on powered status and tile type
        Color wireColor;
        if (tile.getType() == TileType.PC) {
            wireColor = tile.isPowered() ? PC_POWERED_COLOR : PC_UNPOWERED_COLOR;
        } else {
            wireColor = tile.isPowered() ? WIRE_POWERED_COLOR : WIRE_UNPOWERED_COLOR;
        }

        gc.setStroke(wireColor);
        gc.setLineWidth(lineWidth);
        gc.setLineCap(javafx.scene.shape.StrokeLineCap.ROUND);

        switch (tile.getType()) {
            case STRAIGHT:
                drawStraight(gc);
                break;
            case CORNER:
                drawCorner(gc);
                break;
            case T_JUNCTION:
                drawTJunction(gc);
                break;
            case CROSS:
                drawCross(gc);
                break;
            case PC:
                drawPC(gc, wireColor);
                break;
            case POWER:
                drawPower(gc);
                break;
            case EMPTY:
                // No drawing needed
                break;
        }

        gc.restore();

        // Draw lock indicator
        if (tile.isLocked() && tile.getType() != TileType.POWER) {
            gc.setFill(Color.rgb(200, 200, 200, 0.5));
            double lockSize = size * 0.2;
            gc.fillOval(size - lockSize - 5, size - lockSize - 5, lockSize, lockSize);
        }
    }

    private void drawStraight(GraphicsContext gc) {
        double center = size / 2;
        gc.strokeLine(center, 0, center, size);
    }

    private void drawCorner(GraphicsContext gc) {
        double center = size / 2;
        gc.strokeLine(center, 0, center, center);
        gc.strokeLine(center, center, size, center);
    }

    private void drawTJunction(GraphicsContext gc) {
        double center = size / 2;
        // N, E, S connections
        gc.strokeLine(center, 0, center, size); // N to S
        gc.strokeLine(center, center, size, center); // center to E
    }

    private void drawCross(GraphicsContext gc) {
        double center = size / 2;
        gc.strokeLine(center, 0, center, size); // N to S
        gc.strokeLine(0, center, size, center); // W to E
    }

    private void drawPC(GraphicsContext gc, Color wireColor) {
        double center = size / 2;

        // Draw wire connection (vertical line from top to center)
        gc.strokeLine(center, 0, center, center - (size * 0.125));

        // Draw PC as a filled square (like in reference game)
        double squareSize = size * 0.375;
        gc.setFill(wireColor);
        gc.fillRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);

        // Add border to make it stand out
        gc.setStroke(wireColor.brighter());
        gc.setLineWidth(lineWidth * 0.3);
        gc.strokeRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);
    }

    private void drawPower(GraphicsContext gc) {
        double center = size / 2;
        
        // Dynamically draw connections for the power source
        boolean[] conns = tile.getConnections();
        if (conns != null) {
            // N, E, S, W
            if (conns.length > 0 && conns[0]) gc.strokeLine(center, 0, center, center);
            if (conns.length > 1 && conns[1]) gc.strokeLine(center, center, size, center);
            if (conns.length > 2 && conns[2]) gc.strokeLine(center, center, center, size);
            if (conns.length > 3 && conns[3]) gc.strokeLine(0, center, center, center);
        }

        // Draw power as filled square (black/dark)
        double squareSize = size * 0.375;
        gc.setFill(Color.BLACK);
        gc.fillRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);

        // Add bright border
        gc.setStroke(Color.rgb(255, 255, 0)); // Yellow border for power
        gc.setLineWidth(lineWidth * 0.5);
        gc.strokeRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);

        // Draw lightning bolt symbol inside
        gc.setFill(Color.rgb(255, 255, 0));
        double boltScale = size / 80.0;
        double[] xPoints = {center - 3 * boltScale, center - 6 * boltScale, center, center + 3 * boltScale, center + 6 * boltScale, center};
        double[] yPoints = {center - 8 * boltScale, center - 2 * boltScale, center - 2 * boltScale, center + 8 * boltScale, center + 2 * boltScale, center + 2 * boltScale};
        gc.fillPolygon(xPoints, yPoints, 6);
    }

    public Tile getTile() {
        return tile;
    }

    public int getRow() {
        return row;
    }

    public int getCol() {
        return col;
    }

    public void updateTile(Tile tile) {
        this.tile = tile;
        this.currentVisualRotation = tile.getRotation();
        draw();
    }
}