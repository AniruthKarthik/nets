#include <iostream>
#include <fstream>
#include <string>

// Include the combined header-only files
#include "cpp/Tile.hpp"
#include "cpp/JsonUtils.hpp"
#include "cpp/GraphBuilder.hpp"
#include "cpp/ConnectivityCheck.hpp"
#include "cpp/GameLogic.hpp"
#include "cpp/CpuStrategy.hpp"
#include "cpp/JsonImporter.hpp"
#include "cpp/JsonExporter.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        json inputJson;
        if (argc > 1) {
            string inputFile = argv[1];
            ifstream i(inputFile);
            inputJson = json::parse(i);
        } else {
            inputJson = json::parse(cin);
        }

        int width = inputJson["meta"]["width"];
        int height = inputJson["meta"]["height"];
        bool wraps = inputJson["meta"]["wraps"];

        GameState state(width, height, wraps);
        state.status = stringToStatus(inputJson["meta"]["status"]);
        state.turn = static_cast<int>(stringToActor(inputJson["meta"]["turn"]));
        
        auto gridJson = inputJson["grid"];
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                auto tObj = gridJson[r][c];
                TileType type = stringToTileType(tObj["type"]);
                int rotation = tObj["rotation"];
                bool locked = tObj.contains("locked") ? tObj["locked"].get<bool>() : false;
                state.board.grid[r][c] = Tile(type, rotation, locked);
                
                if (tObj.contains("connections")) {
                    state.board.grid[r][c].customConnections = tObj["connections"].get<vector<bool>>();
                }

                if (type == POWER) state.board.powerTile = {r, c};
            }
        }

        // 3. Strategy (CPU Turn)
        if (state.turn == CPU) { // Force move if it's CPU turn, ignore solved check (controller handles that)
             Move bestMove = chooseBestMove(state.board);
             
             json response;
             response["move"] = {
                 {"row", bestMove.x},
                 {"col", bestMove.y},
                 {"rotation", bestMove.rotation}
             };
             cout << response << endl;
        } else {
             cout << "{}" << endl;
        }

    } catch (const std::exception& e) {
        // Return empty object on error as per constraint, or log to stderr
        // cerr << "Error: " << e.what() << endl;
        cout << "{}" << endl;
        return 1;
    }
    return 0;
}
