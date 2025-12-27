#include <fstream>
#include <iostream>
#include <string>

// Include the combined header-only files
#include "cpp/ConnectivityCheck.hpp"
#include "cpp/CpuStrategy.hpp"
#include "cpp/GameLogic.hpp"
#include "cpp/GraphBuilder.hpp"
#include "cpp/JsonExporter.hpp"
#include "cpp/JsonImporter.hpp"
#include "cpp/JsonUtils.hpp"
#include "cpp/Tile.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  try {
    json request;
    if (argc > 1) {
      string inputFile = argv[1];
      ifstream i(inputFile);
      request = json::parse(i);
    } else {
      request = json::parse(cin);
    }

    string action = request["action"];
    json inputJson = request["gameState"];

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
        bool locked =
            tObj.contains("locked") ? tObj["locked"].get<bool>() : false;
        state.board.grid[r][c] = Tile(type, rotation, locked);

        if (tObj.contains("connections")) {
          state.board.grid[r][c].customConnections =
              tObj["connections"].get<vector<bool>>();
        }

        if (type == POWER)
          state.board.powerTile = {r, c};
      }
    }

    pair<int, int> lastMovedTile = {-1, -1};
    if (inputJson.contains("lastMove") && !inputJson["lastMove"].is_null()) {
      auto moveJson = inputJson["lastMove"];
      if (moveJson.contains("row") && moveJson.contains("col")) {
        lastMovedTile = {moveJson["row"].get<int>(),
                         moveJson["col"].get<int>()};
      }
    }

    json response;

    if (action == "get_cpu_move") {
      Move bestMove = chooseBestMove(state.board, lastMovedTile);
      response["move"] = {{"row", bestMove.x},
                          {"col", bestMove.y},
                          {"rotation", bestMove.rotation}};
    } else if (action == "get_stats") {
      Graph graph = buildGraph(state.board);
      int components = countComponents(graph);
      int looseEnds = countLooseEnds(state.board);
      bool solved = isSolved(state.board);

      response["stats"] = {{"components", components},
                           {"looseEnds", looseEnds},
                           {"solved", solved}};
    }

    cout << response << endl;

  } catch (const std::exception &e) {
    // Return empty object on error as per constraint, or log to stderr
    // cerr << "Error: " << e.what() << endl;
    cout << "{}" << endl;
    return 1;
  }
  return 0;
}
