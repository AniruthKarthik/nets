#ifndef JSON_IMPORTER_HPP
#define JSON_IMPORTER_HPP

#include "JsonUtils.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Implementation

inline GameState importGameState(const std::string& filename) {
    std::ifstream i(filename);
    if (!i.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    json j = json::parse(i, nullptr, true, true);

    if (!j.contains("meta")) {
        throw std::runtime_error("Missing 'meta' in JSON");
    }

    int width = j["meta"]["width"];
    int height = j["meta"]["height"];
    bool wraps = j["meta"].contains("wraps") ? j["meta"]["wraps"].get<bool>() : false;

    GameState state(width, height, wraps);

    if (j["meta"].contains("status")) {
        state.status = stringToStatus(j["meta"]["status"]);
    }
    if (j["meta"].contains("turn")) {
        state.turn = static_cast<int>(stringToActor(j["meta"]["turn"]));
    }
    
    // Handle both camelCase and snake_case for last move
    json lm;
    if (j.contains("last_move") && !j["last_move"].is_null()) {
        lm = j["last_move"];
    } else if (j.contains("lastMove") && !j["lastMove"].is_null()) {
        lm = j["lastMove"];
    }

    if (!lm.is_null()) {
        if (lm.contains("actor")) state.lastMove.actor = stringToActor(lm["actor"]);
        if (lm.contains("row")) state.lastMove.row = lm["row"];
        if (lm.contains("col")) state.lastMove.col = lm["col"];
        if (lm.contains("rotation")) state.lastMove.rotation = lm["rotation"];
    }

    if (j.contains("stats")) {
        if (j["stats"].contains("loose_ends")) state.looseEnds = j["stats"]["loose_ends"];
        else if (j["stats"].contains("looseEnds")) state.looseEnds = j["stats"]["looseEnds"];
        
        if (j["stats"].contains("components")) state.components = j["stats"]["components"];
        
        if (j["stats"].contains("is_solved")) state.isSolved = j["stats"]["is_solved"];
        else if (j["stats"].contains("solved")) state.isSolved = j["stats"]["solved"];
    }

    if (j.contains("grid")) {
        auto grid = j["grid"];
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                if (r >= grid.size() || c >= grid[r].size()) continue;
                
                auto tObj = grid[r][c];
                TileType type = tObj.contains("type") ? stringToTileType(tObj["type"]) : EMPTY;
                int rotation = tObj.contains("rotation") ? tObj["rotation"].get<int>() : 0;
                bool locked = tObj.contains("locked") ? tObj["locked"].get<bool>() : false;
                state.board.at(r, c) = Tile(type, rotation, locked);
                
                if (tObj.contains("connections")) {
                    state.board.at(r, c).customConnections = tObj["connections"].get<vector<bool>>();
                }
                
                if (type == POWER) {
                    state.board.powerTile = {r, c};
                }
            }
        }
    }

    return state;
}

#endif // JSON_IMPORTER_HPP
