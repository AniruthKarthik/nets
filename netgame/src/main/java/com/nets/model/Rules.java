package com.nets.model;

import java.util.Map;

public class Rules {
    private boolean allow_loops;
    private Map<String, int[]> rotation_rules;

    public Rules() {}

    public boolean isAllowLoops() {
        return allow_loops;
    }

    public void setAllowLoops(boolean allow_loops) {
        this.allow_loops = allow_loops;
    }

    public Map<String, int[]> getRotationRules() {
        return rotation_rules;
    }

    public void setRotationRules(Map<String, int[]> rotation_rules) {
        this.rotation_rules = rotation_rules;
    }
}