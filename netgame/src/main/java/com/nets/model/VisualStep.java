package com.nets.model;

public class VisualStep {
    private int row;
    private int col;
    private int rotation;
    private String type; // "TRY", "UNDO", "SUCCESS", "FAIL", "CONSIDER", "SCORE"
    private double score;

    public VisualStep() {}

    public int getRow() { return row; }
    public int getCol() { return col; }
    public int getRotation() { return rotation; }
    public String getType() { return type; }
    public double getScore() { return score; }
}
