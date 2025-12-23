#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include <string>
#include "Tile.hpp"

using namespace std;

enum GameStatus { PLAYING, WON, LOST };
enum Actor { NONE, HUMAN, CPU };

struct LastMove {
    Actor actor;
    int row;
    int col;
    int rotation;
};

struct GameState {
    Board board;
    GameStatus status;
    int turn;
    LastMove lastMove;
    int looseEnds;
    int components;
    bool isSolved;
    
    // Constructor matching Board
    GameState(int w, int h, bool wrap) : board(w, h, wrap), status(PLAYING), turn(0), looseEnds(0), components(0), isSolved(false) {
        lastMove = {NONE, -1, -1, 0};
    }
};

// Implementations

inline string tileTypeToString(TileType t) {
    switch (t) {
        case EMPTY: return "EMPTY";
        case POWER: return "POWER";
        case PC: return "PC";
        case STRAIGHT: return "STRAIGHT";
        case CORNER: return "CORNER";
        case T_JUNCTION: return "T_JUNCTION";
        case CROSS: return "CROSS";
        default: return "EMPTY";
    }
}

inline TileType stringToTileType(const string& s) {
    if (s == "EMPTY") return EMPTY;
    if (s == "POWER") return POWER;
    if (s == "PC") return PC;
    if (s == "STRAIGHT") return STRAIGHT;
    if (s == "CORNER") return CORNER;
    if (s == "T_JUNCTION") return T_JUNCTION;
    if (s == "CROSS") return CROSS;
    return EMPTY;
}

inline string statusToString(GameStatus s) {
    switch (s) {
        case PLAYING: return "PLAYING";
        case WON: return "SOLVED";
        case LOST: return "FAILED";
        default: return "PLAYING";
    }
}

inline GameStatus stringToStatus(const string& s) {
    if (s == "WON" || s == "SOLVED") return WON;
    if (s == "LOST" || s == "FAILED") return LOST;
    return PLAYING;
}

inline string actorToString(Actor a) {
    switch (a) {
        case HUMAN: return "HUMAN";
        case CPU: return "CPU";
        default: return "NONE";
    }
}

inline Actor stringToActor(const string& s) {
    if (s == "HUMAN") return HUMAN;
    if (s == "CPU") return CPU;
    return NONE;
}

#endif // JSON_UTILS_HPP
