package com.nets.view;

import com.nets.model.Tile;
import com.nets.model.TileType;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.transform.Rotate;

public class TileView extends Canvas {
    private static final double SIZE = 80;
    private static final double LINE_WIDTH = 6;

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

    public TileView(Tile tile, int row, int col) {
        super(SIZE, SIZE);
        this.tile = tile;
        this.row = row;
        this.col = col;
        draw();
    }

    public void draw() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0, 0, SIZE, SIZE);

        // Background
        gc.setFill(tile.isLocked() ? LOCKED_COLOR : BG_COLOR);
        gc.fillRect(0, 0, SIZE, SIZE);

        // Border
        gc.setStroke(Color.rgb(80, 80, 100));
        gc.setLineWidth(1);
        gc.strokeRect(0, 0, SIZE, SIZE);

        // Draw tile based on type and rotation
        gc.save();
        gc.translate(SIZE / 2, SIZE / 2);
        gc.rotate(tile.getRotation());
        gc.translate(-SIZE / 2, -SIZE / 2);

        // Choose color based on powered status and tile type
        Color wireColor;
        if (tile.getType() == TileType.PC) {
            wireColor = tile.isPowered() ? PC_POWERED_COLOR : PC_UNPOWERED_COLOR;
        } else {
            wireColor = tile.isPowered() ? WIRE_POWERED_COLOR : WIRE_UNPOWERED_COLOR;
        }

        gc.setStroke(wireColor);
        gc.setLineWidth(LINE_WIDTH);
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
        if (tile.isLocked()) {
            gc.setFill(Color.rgb(200, 200, 200, 0.5));
            gc.fillOval(SIZE - 20, SIZE - 20, 15, 15);
        }
    }

    private void drawStraight(GraphicsContext gc) {
        double center = SIZE / 2;
        gc.strokeLine(center, 0, center, SIZE);
    }

    private void drawCorner(GraphicsContext gc) {
        double center = SIZE / 2;
        gc.strokeLine(center, 0, center, center);
        gc.strokeLine(center, center, SIZE, center);
    }

    private void drawTJunction(GraphicsContext gc) {
        double center = SIZE / 2;
        // Draw T-shape as one continuous path: top, left, and right (no bottom)
        gc.beginPath();
        gc.moveTo(center, 0);           // Start at top
        gc.lineTo(center, center);      // Draw to center
        gc.moveTo(0, center);           // Move to left
        gc.lineTo(SIZE, center);        // Draw horizontal line to right
        gc.stroke();
    }

    private void drawPC(GraphicsContext gc, Color wireColor) {
        double center = SIZE / 2;

        // Draw wire connection (vertical line from top to center)
        gc.strokeLine(center, 0, center, center - 10);

        // Draw PC as a filled square (like in reference game)
        double squareSize = 30;
        gc.setFill(wireColor);
        gc.fillRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);

        // Add border to make it stand out
        gc.setStroke(wireColor.brighter());
        gc.setLineWidth(2);
        gc.strokeRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);
    }

    private void drawPower(GraphicsContext gc) {
        double center = SIZE / 2;

        // Draw power as filled square (black/dark)
        double squareSize = 30;
        gc.setFill(Color.BLACK);
        gc.fillRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);

        // Add bright border
        gc.setStroke(Color.rgb(255, 255, 0)); // Yellow border for power
        gc.setLineWidth(3);
        gc.strokeRect(center - squareSize/2, center - squareSize/2, squareSize, squareSize);

        // Draw lightning bolt symbol inside
        gc.setFill(Color.rgb(255, 255, 0));
        double[] xPoints = {center - 3, center - 6, center, center + 3, center + 6, center};
        double[] yPoints = {center - 8, center - 2, center - 2, center + 8, center + 2, center + 2};
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
        draw();
    }
}