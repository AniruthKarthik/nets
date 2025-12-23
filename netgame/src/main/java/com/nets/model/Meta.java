package com.nets.model;

import java.util.Map;

public class Meta {
    private int width;
    private int height;
    private int seed;
    private String status;
    private String turn;
    private boolean wraps;

    public Meta() {}

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public int getSeed() {
        return seed;
    }

    public void setSeed(int seed) {
        this.seed = seed;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getTurn() {
        return turn;
    }

    public void setTurn(String turn) {
        this.turn = turn;
    }

    public boolean isWraps() {
        return wraps;
    }

    public void setWraps(boolean wraps) {
        this.wraps = wraps;
    }
}