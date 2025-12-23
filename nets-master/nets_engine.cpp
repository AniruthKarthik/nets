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
    string inputFile = "json/board_state.jsonc";
    string outputFile = "json/board_state.jsonc";

    if (argc > 1) inputFile = argv[1];
    if (argc > 2) outputFile = argv[2];

    try {
        // 1. Import
        // cout << "Loading state from " << inputFile << "..." << endl;
        GameState state = importGameState(inputFile);

        // 2. Analysis
        state.looseEnds = countLooseEnds(state.board);
        state.components = countComponents(state.board);
        state.isSolved = isSolved(state.board);

        // cout << "Analysis: " << state.looseEnds << " loose ends, " 
        //      << state.components << " components. Solved: " << (state.isSolved ? "Yes" : "No") << endl;

        // 3. Strategy (CPU Turn)
        if (state.turn == CPU && !state.isSolved && state.status == PLAYING) {
            //  cout << "CPU executing move..." << endl;
             Move bestMove = chooseBestMove(state.board);
             
            //  cout << "Applying move: (" << bestMove.x << ", " << bestMove.y << ") rotate " << bestMove.rotation << endl;
             applyMove(state.board, bestMove);
             
             state.lastMove.actor = CPU;
             state.lastMove.row = bestMove.x;
             state.lastMove.col = bestMove.y;
             state.lastMove.rotation = bestMove.rotation;

             state.turn = HUMAN; 

             // Re-evaluate
             state.looseEnds = countLooseEnds(state.board);
             state.components = countComponents(state.board);
             state.isSolved = isSolved(state.board);
             
             if (state.isSolved) {
                 state.status = WON;
                //  cout << "Puzzle Solved by CPU!" << endl;
             }
        }

        // 4. Export
        // cout << "Saving state to " << outputFile << "..." << endl;
        exportGameState(state, outputFile);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
