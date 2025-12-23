package com.nets.model;

public class Stats {
    private int components;
    private boolean is_solved;
    private int loose_ends;

    public Stats() {}

    public int getComponents() {
        return components;
    }

    public void setComponents(int components) {
        this.components = components;
    }

    public boolean isSolved() {
        return is_solved;
    }

    public void setSolved(boolean is_solved) {
        this.is_solved = is_solved;
    }

    public int getLooseEnds() {
        return loose_ends;
    }

    public void setLooseEnds(int loose_ends) {
        this.loose_ends = loose_ends;
    }
}