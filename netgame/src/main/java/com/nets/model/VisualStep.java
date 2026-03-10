package com.nets.model;

public class VisualStep {
    private int row;
    private int col;
    private int rotation;
    private String type; // "TRY", "UNDO", "SUCCESS", "FAIL", "CONSIDER", "SCORE", "REGION"
    private double score;
    private int upmask;
    private int leftreq;
    private int r0 = -1, r1 = -1, c0 = -1, c1 = -1;

    public VisualStep() {}

    public int getRow() { return row; }
    public int getCol() { return col; }
    public int getRotation() { return rotation; }
    public String getType() { return type; }
    public double getScore() { return score; }
    public int getUpmask() { return upmask; }
    public int getLeftreq() { return leftreq; }
    public int getR0() { return r0; }
    public int getR1() { return r1; }
    public int getC0() { return c0; }
    public int getC1() { return c1; }
}
