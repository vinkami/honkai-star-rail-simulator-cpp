//
// Created by vinkami on 11/4/2024.
//

#ifndef PROJECT_BATTLE_H
#define PROJECT_BATTLE_H

#include "state.h"
#include <vector>
using namespace std;

void battleStart(State &state);
bool gameLoop(State &state);
void battleEnd(State &state);

#endif //PROJECT_BATTLE_H
