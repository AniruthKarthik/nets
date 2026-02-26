#include <fstream>
#include <iostream>
#include <string>

// Include the combined header-only files
#include "cpp/ConnectivityCheck.hpp"
#include "cpp/CpuStrategy.hpp"
#include "cpp/DpSolver.hpp"
#include "cpp/BtSolver.hpp"
#include "cpp/DacSolver.hpp"
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
      Move bestMove = chooseBestMove_greedy(state.board, lastMovedTile);
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
    } else if (action == "solve_game") {
      string solverType = request.contains("solver") ? request["solver"].get<string>() : "dp";
      bool success = false;
      string implementation = "";

      if (solverType == "bt") {
          implementation = "Backtracking (BT)";
          cerr << "[Engine] Solving using: " << implementation << endl;
          success = solve_bt(state.board);
      } else if (solverType == "dac") {
          implementation = "Divide and Conquer (DAC)";
          cerr << "[Engine] Solving using: " << implementation << endl;
          success = solve_dac(state.board);
      } else {
          implementation = "Dynamic Programming (DP)";
          cerr << "[Engine] Solving using: " << implementation << endl;
          success = solve_dp(state.board);
      }

      response["solved"] = success;
      response["implementation"] = implementation;
      
      if (success) {
        json solvedGrid = json::array();
        for (int r = 0; r < height; r++) {
          json row = json::array();
          for (int c = 0; c < width; c++) {
            json tile;
            tile["rotation"] = state.board.grid[r][c].rotation;
            row.push_back(tile);
          }
          solvedGrid.push_back(row);
        }
        response["grid"] = solvedGrid;
      }
    }

    cout << response << endl;

  } catch (const std::exception &e) {
    cerr << "Error: " << e.what() << endl;
    cout << "{}" << endl;
    return 1;
  }
  return 0;
}
