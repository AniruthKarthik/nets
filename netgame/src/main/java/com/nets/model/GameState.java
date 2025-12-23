package com.nets.model;

public class GameState {
    private Tile[][] grid;
    private Move last_move;
    private Meta meta;
    private Stats stats;
    private Rules rules;

    public GameState() {}

    public Tile[][] getGrid() {
        return grid;
    }

    public void setGrid(Tile[][] grid) {
        this.grid = grid;
    }

    public Move getLastMove() {
        return last_move;
    }

    public void setLastMove(Move last_move) {
        this.last_move = last_move;
    }

    public Meta getMeta() {
        return meta;
    }

    public void setMeta(Meta meta) {
        this.meta = meta;
    }

    public Stats getStats() {
        return stats;
    }

    public void setStats(Stats stats) {
        this.stats = stats;
    }

    public Rules getRules() {
        return rules;
    }

    public void setRules(Rules rules) {
        this.rules = rules;
    }
}