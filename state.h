//
// Created by vinkami on 11/4/2024.
//

#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include <vector>
using namespace std;

class Character;
class State {
public:
    State(vector<Character> &allies, vector<Character> &enemies, Character &round);
    vector<Character> &allies;
    vector<Character> &enemies;
    Character &round;

    int maxSkillPoint;
    int skillPoint;
    int roundNumber;

    void forward(double time);
    Character nextCharacter();
};

#endif //PROJECT_STATE_H
