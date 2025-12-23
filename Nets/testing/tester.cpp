#include <iostream>
#include <vector>
#include <cassert>
#include "../cpp/Tile.hpp"
#include "../cpp/CpuStrategy.hpp"
#include "../cpp/GameLogic.hpp"
#include "../cpp/ConnectivityCheck.hpp"

using namespace std;

// Helper to print board
void printBoard(const Board &board) {
    cout << "Board (" << board.width << "x" << board.height << "):" << endl;
    for (int r = 0; r < board.height; ++r) {
        for (int c = 0; c < board.width; ++c) {
            const Tile &t = board.grid[r][c];
            cout << "[" << (int)t.type << ":" << t.rotation << "] ";
        }
        cout << endl;
    }
}

// Helper to set tile
void setTile(Board &board, int r, int c, TileType type, int rot) {
    board.grid[r][c] = Tile(type, rot);
}

void test_rotations() {
    cout << "Testing Rotations..." << endl;
    Tile t(STRAIGHT, 0); // North-South
    vector<Direction> ports = getActivePorts(t);
    // Expect NORTH(0) and SOUTH(2)
    bool hasN = false, hasS = false;
    for(auto d : ports) {
        if(d == NORTH) hasN = true;
        if(d == SOUTH) hasS = true;
    }
    assert(hasN && hasS && ports.size() == 2);

    t.rotation = 90; // East-West
    ports = getActivePorts(t);
    // Expect EAST(1) and WEST(3)
    bool hasE = false, hasW = false;
    for(auto d : ports) {
        if(d == EAST) hasE = true;
        if(d == WEST) hasW = true;
    }
    assert(hasE && hasW && ports.size() == 2);
    cout << "Rotations OK" << endl;
}

void test_simple_connection() {
    cout << "Testing Simple Connection..." << endl;
    Board b(2, 1);
    // [STRAIGHT 90] [STRAIGHT 90]  ->  - -
    setTile(b, 0, 0, STRAIGHT, 90);
    setTile(b, 0, 1, STRAIGHT, 90);
    
    int loose = countLooseEnds(b);
    // Both tiles: East, West.
    // (0,0) East connects to (0,1) West.
    // (0,0) West is loose (boundary).
    // (0,1) East is loose (boundary).
    // Total loose ends: 2.
    assert(loose == 2);

    int comps = countComponents(b);
    assert(comps == 1); // Connected

    cout << "Simple Connection OK" << endl;
}

void test_cpu_strategy_2x2() {
    cout << "Testing CPU Strategy 2x2..." << endl;
    
    // Case: [STRAIGHT 90 (-)] [STRAIGHT 0 (|)]
    // Loose: 4 (L, R for 1st; T, B for 2nd - assuming boundary is loose).
    // Wait.
    // Tile 1: E, W. E connects to Tile 2? Tile 2 is | (N, S).
    // Tile 1 E neighbor is Tile 2. Tile 2 ports: N, S.
    // Tile 1 E matches Tile 2 W? Tile 2 has no W.
    // So connection fails.
    // Tile 1 W is loose (boundary).
    // Tile 1 E is loose (no match).
    // Tile 2 N is loose.
    // Tile 2 S is loose.
    // Total loose: 4.
    // Components: 2.
    // Score: 40 + 10 = 50.
    
    // Move (0,1) + 90 -> (-).
    // Tile 1: E, W. Tile 2: E, W.
    // Tile 1 E connects to Tile 2 W.
    // Tile 1 W loose.
    // Tile 2 E loose.
    // Loose: 2. Components: 1.
    // Score: 20 + 5 = 25.
    
    // Move (0,0) + 90 -> (|).
    // Tile 1: N, S. Tile 2: N, S.
    // No connection.
    // Loose: 4. Comps: 2. Score 50.
    
    Board b(2, 1);
    setTile(b, 0, 0, STRAIGHT, 90);
    setTile(b, 0, 1, STRAIGHT, 0);
    
    Move m = chooseBestMove(b);
    cout << "Best move for - | : " << m.x << "," << m.y << " rot " << m.rotation << endl;
    
    // Expect m.x=0, m.y=1, m.rotation=90 (or 270).
    assert(m.x == 0 && m.y == 1);
    assert(m.rotation == 90 || m.rotation == 270);
    
    cout << "CPU Strategy 2x2 OK" << endl;
}

void test_full_game_simulation() {
    cout << "Testing Full Game Simulation..." << endl;
    
    // Solvable 1x2:
    // [PC 90 (E)] [PC 270 (W)]
    // (0,0) E connects to (0,1) W.
    // No other ports.
    // Loose ends: 0.
    // Components: 1.
    // Loop: False.
    // Win!
    
    Board b3(2, 1); // 2 width, 1 height
    setTile(b3, 0, 0, POWER, 0); // N (bad)
    b3.powerTile = {0,0};
    setTile(b3, 0, 1, PC, 0); // N (bad)
    
    cout << "Initial board state:" << endl;
    printBoard(b3);

    // CPU should solve this.
    int limit = 10;
    while(!isSolved(b3) && limit-- > 0) {
        Move m = chooseBestMove(b3);
        cout << "Chosen move: " << m.x << "," << m.y << " rot " << m.rotation << endl;
        applyMove(b3, m);
        printBoard(b3);
        cout << "Loose: " << countLooseEnds(b3) << " Comps: " << countComponents(b3) << " Win: " << isSolved(b3) << endl;
    }
    
    assert(isSolved(b3));
    cout << "Full Game Simulation OK" << endl;
}

void test_loop_stuck() {
    cout << "Testing Loop Stuck Bug..." << endl;
    // 2x2 Loop
    // (0,0) CORNER 90 (E,S)
    // (0,1) CORNER 180 (S,W)
    // (1,0) CORNER 0 (N,E)
    // (1,1) CORNER 270 (W,N)
    // All connected. Loop.
    
    Board b(2, 2);
    setTile(b, 0, 0, CORNER, 90);
    setTile(b, 0, 1, CORNER, 180);
    setTile(b, 1, 0, CORNER, 0);
    setTile(b, 1, 1, CORNER, 270);
    
    assert(hasClosedLoop(b));
    assert(countLooseEnds(b) == 0);
    
    // Check best move
    Move m = chooseBestMove(b);
    Board b_next = b;
    applyMove(b_next, m);
    
    int scoreBefore = evaluateBoard(b);
    int scoreAfter = evaluateBoard(b_next);
    
    cout << "Score Before: " << scoreBefore << endl;
    cout << "Score After: " << scoreAfter << endl;
    
    // With fix, scoreBefore (Loop) should be very high (e.g. > 1000).
    // scoreAfter should be low (e.g. 25).
    // So CPU will definitely pick the move to break the loop.
    
    if (scoreBefore > 100) {
        cout << "Loop penalty detected." << endl;
    } else {
        cout << "WARNING: Loop penalty NOT detected." << endl;
    }

    cout << "Simulating oscillation..." << endl;
    for(int i=0; i<4; ++i) {
        Move m = chooseBestMove(b);
        applyMove(b, m);
        int score = evaluateBoard(b);
        bool loop = hasClosedLoop(b);
        cout << "Turn " << i << " Score: " << score << " Loop: " << loop << endl;
        
        // Assert that we don't return to loop state
        assert(!loop); 
        // Assert score is reasonable
        assert(score < 100);
    }
}

int main() {
    test_rotations();
    test_simple_connection();
    test_cpu_strategy_2x2();
    test_full_game_simulation();
    test_loop_stuck();
    cout << "All Tests Passed!" << endl;
    return 0;
}
