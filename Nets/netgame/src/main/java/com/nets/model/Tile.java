package com.nets.model;

public class Tile {
    private TileType type;
    private int rotation;
    private boolean locked;
    private boolean isPowered;
    private boolean[] connections;

    public Tile() {}

    public Tile(TileType type, int rotation, boolean locked) {
        this.type = type;
        this.rotation = rotation;
        this.locked = locked;
        this.isPowered = false;
        this.connections = null;
    }

    public boolean[] getConnections() {
        return connections;
    }

    public void setConnections(boolean[] connections) {
        this.connections = connections;
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

    public boolean isPowered() {
        return isPowered;
    }

    public void setPowered(boolean powered) {
        isPowered = powered;
    }

    public void rotate(int degrees) {
        if (!locked) {
            this.rotation = (this.rotation + degrees) % 360;
            if (this.rotation < 0) {
                this.rotation += 360;
            }
        }
    }
}
