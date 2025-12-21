#include "JsonUtils.h"
#include <stdexcept>

string tileTypeToString(TileType t) {
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

TileType stringToTileType(const string& s) {
    if (s == "EMPTY") return EMPTY;
    if (s == "POWER") return POWER;
    if (s == "PC") return PC;
    if (s == "STRAIGHT") return STRAIGHT;
    if (s == "CORNER") return CORNER;
    if (s == "T_JUNCTION") return T_JUNCTION;
    if (s == "CROSS") return CROSS;
    return EMPTY;
}

string statusToString(GameStatus s) {
    switch (s) {
        case PLAYING: return "PLAYING";
        case WON: return "WON";
        case LOST: return "LOST";
        default: return "PLAYING";
    }
}

GameStatus stringToStatus(const string& s) {
    if (s == "WON") return WON;
    if (s == "LOST") return LOST;
    return PLAYING;
}

string actorToString(Actor a) {
    switch (a) {
        case HUMAN: return "HUMAN";
        case CPU: return "CPU";
        default: return "NONE";
    }
}

Actor stringToActor(const string& s) {
    if (s == "HUMAN") return HUMAN;
    if (s == "CPU") return CPU;
    return NONE;
}
