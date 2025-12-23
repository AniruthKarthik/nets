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
    private static final Color WIRE_COLOR = Color.rgb(255, 215, 0); // Gold
    private static final Color BG_COLOR = Color.rgb(40, 40, 60);
    private static final Color LOCKED_COLOR = Color.rgb(60, 60, 80);
    private static final Color POWER_COLOR = Color.rgb(0, 255, 100);

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

        gc.setStroke(tile.isPowered() ? POWER_COLOR : WIRE_COLOR);
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
                drawPC(gc);
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
        gc.strokeLine(center, 0, center, SIZE);
        gc.strokeLine(0, center, SIZE, center);
        // Block bottom
        gc.setStroke(BG_COLOR);
        gc.setLineWidth(LINE_WIDTH + 2);
        gc.strokeLine(center, SIZE - LINE_WIDTH, center, SIZE);
    }

    private void drawPC(GraphicsContext gc) {
        double center = SIZE / 2;
        gc.strokeLine(center, 0, center, SIZE);

        // Draw computer screen
        gc.setFill(Color.rgb(100, 150, 255));
        gc.fillRect(center - 15, center - 10, 30, 20);
        gc.setStroke(Color.WHITE);
        gc.setLineWidth(2);
        gc.strokeRect(center - 15, center - 10, 30, 20);
    }

    private void drawPower(GraphicsContext gc) {
        double center = SIZE / 2;
        // Draw lightning bolt
        gc.setFill(POWER_COLOR);
        double[] xPoints = {center, center - 5, center, center + 5};
        double[] yPoints = {center - 15, center, center, center + 15};
        gc.fillPolygon(xPoints, yPoints, 4);

        // Draw power circle
        gc.setStroke(POWER_COLOR);
        gc.setLineWidth(3);
        gc.strokeOval(center - 20, center - 20, 40, 40);
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