#include "JsonImporter.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

GameState importGameState(const std::string& filename) {
    std::ifstream i(filename);
    json j;
    i >> j;

    int width = j["metadata"]["width"];
    int height = j["metadata"]["height"];
    bool wraps = j["metadata"]["wraps"];

    GameState state(width, height, wraps);

    state.status = stringToStatus(j["metadata"]["status"]);
    state.turn = j["metadata"]["turn"];
    
    auto lm = j["metadata"]["last_move"];
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
            state.board.grid[r][c] = Tile(type, rotation);
            
            if (type == POWER) {
                state.board.powerTile = {r, c};
            }
        }
    }

    return state;
}
