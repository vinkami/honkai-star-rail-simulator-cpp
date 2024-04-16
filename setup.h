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
#include "characterinfo.h"
#include <iostream>


int targetinlist(const std::vector<Character>& characters, const std::string& target);

vector<Character> teamMenu();

void battleMenu(State &state);

void settingsMenu(State &state);

#endif //PROJECT_SETUP_H
