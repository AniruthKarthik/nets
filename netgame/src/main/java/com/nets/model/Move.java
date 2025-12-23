package com.nets.model;

public class Move {
    private String actor;
    private int row;
    private int col;
    private int rotation;

    public Move() {}

    public Move(String actor, int row, int col, int rotation) {
        this.actor = actor;
        this.row = row;
        this.col = col;
        this.rotation = rotation;
    }

    public String getActor() {
        return actor;
    }

    public void setActor(String actor) {
        this.actor = actor;
    }

    public int getRow() {
        return row;
    }

    public void setRow(int row) {
        this.row = row;
    }

    public int getCol() {
        return col;
    }

    public void setCol(int col) {
        this.col = col;
    }

    public int getRotation() {
        return rotation;
    }

    public void setRotation(int rotation) {
        this.rotation = rotation;
    }
}