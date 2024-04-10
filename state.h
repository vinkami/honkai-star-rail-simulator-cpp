//
// Created by vinkami on 11/4/2024.
//

#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include "character.h"
#include <vector>
using namespace std;

class State {
public:
    State(vector<Character> &allies, vector<Character> &enemies);
    vector<Character> &allies;
    vector<Character> &enemies;
    Character round;

    int maxSkillPoint;
    int skillPoint;
    int roundNumber;

    void forward(double time);
    Character nextCharacter();
};

#endif //PROJECT_STATE_H
