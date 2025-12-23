#ifndef JSON_EXPORTER_HPP
#define JSON_EXPORTER_HPP

#include "JsonUtils.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Implementation

inline void exportGameState(const GameState& state, const std::string& filename) {
    json j;

    j["meta"] = {
        {"width", state.board.width},
        {"height", state.board.height},
        {"wraps", state.board.wraps},
        {"status", statusToString(state.status)},
        {"turn", actorToString(static_cast<Actor>(state.turn))},
        {"seed", 0}
    };

    j["rules"] = {
        {"allow_loops", false},
        {"rotation_rules", {
            {"EMPTY", json::array()},
            {"POWER", {0}},
            {"PC", {0, 90, 180, 270}},
            {"STRAIGHT", {0, 90}},
            {"CORNER", {0, 90, 180, 270}},
            {"T_JUNCTION", {0, 90, 180, 270}},
            {"CROSS", {0}}
        }}
    };

    j["last_move"] = {
        {"actor", actorToString(state.lastMove.actor)},
        {"row", state.lastMove.row},
        {"col", state.lastMove.col},
        {"rotation", state.lastMove.rotation}
    };

    j["stats"] = {
        {"loose_ends", state.looseEnds},
        {"components", state.components},
        {"is_solved", state.isSolved}
    };

    json grid = json::array();
    for (int r = 0; r < state.board.height; ++r) {
        json row = json::array();
        for (int c = 0; c < state.board.width; ++c) {
            const Tile& t = state.board.at(r, c);
            row.push_back({
                {"type", tileTypeToString(t.type)},
                {"rotation", t.rotation},
                {"locked", t.locked}
            });
        }
        grid.push_back(row);
    }
    j["grid"] = grid;

    std::ofstream o(filename);
    o << std::setw(2) << j << std::endl;
}

#endif // JSON_EXPORTER_HPP
