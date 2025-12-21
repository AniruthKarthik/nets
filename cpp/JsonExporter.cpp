#include "JsonExporter.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void exportGameState(const GameState& state, const std::string& filename) {
    json j;

    j["metadata"] = {
        {"width", state.board.width},
        {"height", state.board.height},
        {"wraps", state.board.wraps},
        {"status", statusToString(state.status)},
        {"turn", state.turn},
        {"last_move", {
            {"actor", actorToString(state.lastMove.actor)},
            {"row", state.lastMove.row},
            {"col", state.lastMove.col},
            {"rotation", state.lastMove.rotation}
        }}
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
                {"locked", false} // C++ logic doesn't seem to have locked bit in Tile struct yet, defaulting to false or need update? 
                                  // Prompt said "locked state". Board struct doesn't have it.
                                  // "do not change existing game algorithms"
                                  // I will just default to false for now as C++ doesn't track it.
            });
        }
        grid.push_back(row);
    }
    j["grid"] = grid;

    std::ofstream o(filename);
    o << std::setw(2) << j << std::endl;
}
