#ifndef JSON_IMPORTER_HPP
#define JSON_IMPORTER_HPP

#include "JsonUtils.hpp"
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Implementation

inline GameState importGameState(const std::string& filename) {
    std::ifstream i(filename);
    json j = json::parse(i, nullptr, true, true);

    int width = j["meta"]["width"];
    int height = j["meta"]["height"];
    bool wraps = j["meta"]["wraps"];

    GameState state(width, height, wraps);

    state.status = stringToStatus(j["meta"]["status"]);
    state.turn = static_cast<int>(stringToActor(j["meta"]["turn"]));
    
    auto lm = j["last_move"];
    state.lastMove.actor = stringToActor(lm["actor"]);
    state.lastMove.row = lm["row"];
    state.lastMove.col = lm["col"];
    state.lastMove.rotation = lm["rotation"];

    state.looseEnds = j["stats"]["loose_ends"];
    state.components = j["stats"]["components"];
    state.isSolved = j["stats"]["is_solved"];

    auto grid = j["grid"];
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            auto tObj = grid[r][c];
            TileType type = stringToTileType(tObj["type"]);
            int rotation = tObj["rotation"];
            bool locked = tObj.contains("locked") ? tObj["locked"].get<bool>() : false;
            state.board.grid[r][c] = Tile(type, rotation, locked);
            
            if (tObj.contains("connections")) {
                state.board.grid[r][c].customConnections = tObj["connections"].get<vector<bool>>();
            }
            
            if (type == POWER) {
                state.board.powerTile = {r, c};
            }
        }
    }

    return state;
}

#endif // JSON_IMPORTER_HPP
