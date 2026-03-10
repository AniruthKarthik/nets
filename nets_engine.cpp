#include <fstream>
#include <iostream>
#include <string>
#include <exception>

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
      if (!i.is_open()) {
          cerr << "Error: Could not open input file: " << inputFile << endl;
          return 1;
      }
      request = json::parse(i);
    } else {
      request = json::parse(cin);
    }

    if (!request.contains("action")) {
        cerr << "Error: Missing 'action' in request" << endl;
        return 1;
    }
    string action = request["action"];
    
    if (!request.contains("gameState")) {
        cerr << "Error: Missing 'gameState' in request" << endl;
        return 1;
    }
    json inputJson = request["gameState"];

    if (!inputJson.contains("meta") || !inputJson["meta"].contains("width") || !inputJson["meta"].contains("height")) {
        cerr << "Error: Missing 'meta' or 'width'/'height' in gameState" << endl;
        return 1;
    }

    int width = inputJson["meta"]["width"];
    int height = inputJson["meta"]["height"];
    bool wraps = inputJson["meta"].contains("wraps") ? inputJson["meta"]["wraps"].get<bool>() : false;

    GameState state(width, height, wraps);
    if (inputJson["meta"].contains("status")) {
        state.status = stringToStatus(inputJson["meta"]["status"]);
    }
    if (inputJson["meta"].contains("turn")) {
        state.turn = static_cast<int>(stringToActor(inputJson["meta"]["turn"]));
    }

    if (!inputJson.contains("grid")) {
        cerr << "Error: Missing 'grid' in gameState" << endl;
        return 1;
    }
    auto gridJson = inputJson["grid"];
    for (int r = 0; r < height; ++r) {
      for (int c = 0; c < width; ++c) {
        if (r >= gridJson.size() || c >= gridJson[r].size()) continue;
        
        auto tObj = gridJson[r][c];
        TileType type = tObj.contains("type") ? stringToTileType(tObj["type"]) : EMPTY;
        int rotation = tObj.contains("rotation") ? tObj["rotation"].get<int>() : 0;
        bool locked = tObj.contains("locked") ? tObj["locked"].get<bool>() : false;
        state.board.at(r, c) = Tile(type, rotation, locked);

        if (tObj.contains("connections")) {
          state.board.at(r, c).customConnections =
              tObj["connections"].get<vector<bool>>();
        }

        if (type == POWER)
          state.board.powerTile = {r, c};
      }
    }

    pair<int, int> lastMovedTile = {-1, -1};
    // Support both camelCase and snake_case for last move
    json moveJson;
    if (inputJson.contains("lastMove") && !inputJson["lastMove"].is_null()) {
        moveJson = inputJson["lastMove"];
    } else if (inputJson.contains("last_move") && !inputJson["last_move"].is_null()) {
        moveJson = inputJson["last_move"];
    }

    if (!moveJson.is_null() && moveJson.contains("row") && moveJson.contains("col")) {
        lastMovedTile = {moveJson["row"].get<int>(), moveJson["col"].get<int>()};
    }

    json response;

    if (action == "get_cpu_move") {
      string algo = request.contains("algo") ? request["algo"].get<string>() : "greedy";
      bool visualize = request.contains("visualize") ? request["visualize"].get<bool>() : false;
      Move bestMove = {0, 0, 0};
      vector<VisualStep> steps;

      if (algo == "backtracking" || algo == "dp" || algo == "divideandconquer") {
          Board solvedBoard = state.board;
          bool success = false;
          if (algo == "backtracking") {
              success = solve_bt(solvedBoard, visualize ? &steps : nullptr);
          } else if (algo == "dp") {
              success = solve_dp(solvedBoard, visualize ? &steps : nullptr);
          } else if (algo == "divideandconquer") {
              success = solve_dac(solvedBoard, visualize ? &steps : nullptr);
          }

          if (success) {
              // Find first tile that differs
              bool found = false;
              for (int r = 0; r < height && !found; ++r) {
                  for (int c = 0; c < width && !found; ++c) {
                      int currentRot = state.board.at(r, c).rotation;
                      int targetRot = solvedBoard.at(r, c).rotation;
                      if (currentRot != targetRot) {
                          // Rotate 90 degrees clockwise towards target
                          bestMove = {r, c, (currentRot + 90) % 360};
                          found = true;
                      }
                  }
              }
              if (!found) {
                  bestMove = chooseBestMove_greedy(state.board, lastMovedTile);
              }
          } else {
              bestMove = chooseBestMove_greedy(state.board, lastMovedTile);
          }
      } else {
          bestMove = chooseBestMove_greedy(state.board, lastMovedTile);
      }

      response["move"] = {{"row", bestMove.x},
                          {"col", bestMove.y},
                          {"rotation", bestMove.rotation}};
      if (visualize) {
          json steps_json = json::array();
          for (const auto& s : steps) {
              steps_json.push_back(s.to_json());
          }
          response["steps"] = steps_json;
      }
    } else if (action == "get_visualization_steps") {
        string algo = request.contains("algo") ? request["algo"].get<string>() : "greedy";
        vector<VisualStep> steps;
        
        if (algo == "greedy") {
            for (int r = 0; r < height; ++r) {
                for (int c = 0; c < width; ++c) {
                    if (state.board.at(r, c).locked || state.board.at(r, c).type == EMPTY) continue;
                    
                    int originalRot = state.board.at(r, c).rotation;
                    for (int rot : {0, 90, 180, 270}) {
                        state.board.at(r, c).rotation = rot;
                        double score = (double)evaluateBoard_greedy(state.board);
                        steps.push_back({r, c, rot, "TRY", 0});
                        steps.push_back({r, c, rot, "SCORE", score});
                    }
                    state.board.at(r, c).rotation = originalRot;
                    steps.push_back({r, c, originalRot, "UNDO", 0});
                }
            }
        } else if (algo == "backtracking" || algo == "dp" || algo == "divideandconquer") {
            Board solvedBoard = state.board;
            if (algo == "backtracking") {
                solve_bt(solvedBoard, &steps);
            } else if (algo == "dp") {
                solve_dp(solvedBoard, &steps);
            } else if (algo == "divideandconquer") {
                solve_dac(solvedBoard, &steps);
            }
        }
        
        json steps_json = json::array();
        for (const auto& s : steps) {
            steps_json.push_back(s.to_json());
        }
        response["steps"] = steps_json;

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
          success = solve_bt(state.board);
      } else if (solverType == "dac") {
          implementation = "Divide and Conquer (DAC)";
          success = solve_dac(state.board);
      } else {
          implementation = "Dynamic Programming (DP)";
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
            tile["rotation"] = state.board.at(r, c).rotation;
            row.push_back(tile);
          }
          solvedGrid.push_back(row);
        }
        response["grid"] = solvedGrid;
      }
    }

    cout << response << endl;

  } catch (const std::exception &e) {
    cerr << "Engine Error: " << e.what() << endl;
    cout << "{}" << endl;
    return 1;
  } catch (...) {
    cerr << "Engine Error: Unknown exception" << endl;
    cout << "{}" << endl;
    return 1;
  }
  return 0;
}
