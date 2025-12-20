int main() {
    cout << "=== Net Puzzle Validator (5×5 Board) ===" << endl << endl;
    
    Board board(5, 5, false);
    
    /*
     * Simple Spiral Pattern (Guaranteed No Loose Ends):
     * 
     *   ┌─┬─┬─┬─┐
     *   ├─┘ └─┤ │
     *   │ ┌─⚡ │
     *   │ └─┬─┘ │
     *   └───┴───┘
     */
    
    // Row 0
    board.at(0, 0) = Tile(CORNER, 270);    // ┌ (E,S)
    board.at(0, 1) = Tile(STRAIGHT, 90);   // ─ (E,W)
    board.at(0, 2) = Tile(T_JUNCTION, 180);// ┬ (E,S,W)
    board.at(0, 3) = Tile(STRAIGHT, 90);   // ─ (E,W)
    board.at(0, 4) = Tile(CORNER, 180);    // ┐ (S,W)
    
    // Row 1
    board.at(1, 0) = Tile(T_JUNCTION, 90); // ├ (N,E,S)
    board.at(1, 1) = Tile(CORNER, 90);     // ┘ (N,W)
    board.at(1, 2) = Tile(CORNER, 0);      // └ (N,E)
    board.at(1, 3) = Tile(T_JUNCTION, 270);// ┤ (N,S,W)
    board.at(1, 4) = Tile(STRAIGHT, 0);    // │ (N,S)
    
    // Row 2
    board.at(2, 0) = Tile(STRAIGHT, 0);    // │ (N,S)
    board.at(2, 1) = Tile(CORNER, 270);    // ┌ (E,S)
    board.at(2, 2) = Tile(POWER, 0);       // ⚡ (N,E,S,W)
    board.at(2, 3) = Tile(STRAIGHT, 90);   // ─ (E,W)
    board.at(2, 4) = Tile(STRAIGHT, 0);    // │ (N,S)
    
    // Row 3
    board.at(3, 0) = Tile(STRAIGHT, 0);    // │ (N,S)
    board.at(3, 1) = Tile(CORNER, 0);      // └ (N,E)
    board.at(3, 2) = Tile(T_JUNCTION, 180);// ┬ (E,S,W)
    board.at(3, 3) = Tile(CORNER, 90);     // ┘ (N,W)
    board.at(3, 4) = Tile(STRAIGHT, 0);    // │ (N,S)
    
    // Row 4
    board.at(4, 0) = Tile(CORNER, 0);      // └ (N,E)
    board.at(4, 1) = Tile(STRAIGHT, 90);   // ─ (E,W)
    board.at(4, 2) = Tile(T_JUNCTION, 0);  // ┴ (N,E,W)
    board.at(4, 3) = Tile(STRAIGHT, 90);   // ─ (E,W)
    board.at(4, 4) = Tile(CORNER, 90);     // ┘ (N,W)
    
    board.powerTile = {2, 2};
    
    printBoardVisual(board);
    
    cout << "\nTesting SOLVED 5×5 board:" << endl;
    cout << "=============================" << endl;
    
    Graph graph = buildGraph(board);
    cout << "Graph nodes: " << graph.nodeCount() << endl;
    
    bool connected = isFullyConnected(graph, board.powerTile);
    cout << "Fully connected: " << (connected ? "YES" : "NO") << endl;
    
    int looseEnds = countLooseEnds(board);
    cout << "Loose ends: " << looseEnds << endl;
    
    bool hasLoop = hasClosedLoop(graph, board.powerTile);
    cout << "Has closed loop: " << (hasLoop ? "YES" : "NO") << endl;
    
    cout << endl;
    bool solved = isSolved(board);
    
    return 0;
}
