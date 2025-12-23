package com.nets.model;

public class Tile {
    private TileType type;
    private int rotation;
    private boolean locked;

    public Tile() {}

    public Tile(TileType type, int rotation, boolean locked) {
        this.type = type;
        this.rotation = rotation;
        this.locked = locked;
    }

    public TileType getType() {
        return type;
    }

    public void setType(TileType type) {
        this.type = type;
    }

    public int getRotation() {
        return rotation;
    }

    public void setRotation(int rotation) {
        this.rotation = rotation;
    }

    public boolean isLocked() {
        return locked;
    }

    public void setLocked(boolean locked) {
        this.locked = locked;
    }
}