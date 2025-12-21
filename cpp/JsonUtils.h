#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>
#include "Tile.h"

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

string tileTypeToString(TileType t);
TileType stringToTileType(const string& s);
string statusToString(GameStatus s);
GameStatus stringToStatus(const string& s);
string actorToString(Actor a);
Actor stringToActor(const string& s);

#endif
