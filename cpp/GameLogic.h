#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "CpuStrategy.h"

void applyMove(Board &board, const Move &move);
bool checkWin(const Board &board);

#endif
