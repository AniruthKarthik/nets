#pragma once

// Forward declaration to avoid including the full Tile.hpp here if it's large
struct Board;

class PuzzleGenerator {
public:
    // Generates a new, solvable puzzle on the given board.
    static void generate(Board& board);
};
