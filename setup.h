//
// Created by vinkami on 11/4/2024.
//

#ifndef PROJECT_SETUP_H
#define PROJECT_SETUP_H

#include "state.h"

//
// Created by david on 4/17/2024.
//

#include "state.h"
#include <iostream>


int searchCharacter(const std::vector<Character>& characters, const std::string& target);

void teamMenu(State &state);
void battleMenu(State &state);
void settingsMenu(State &state);
void debugMenu(State &state);
void levelsetting(State &state);
void characterlist(State &state);

#endif //PROJECT_SETUP_H
