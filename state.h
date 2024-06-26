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
    State(vector<Character> &allies, vector<Character> &enemies, Character &round, vector<Character> &alliesOriginal, vector<Character> &enemiesOriginal);
    vector<Character> &allies, &enemies;
    vector<Character> &alliesOriginal, &enemiesOriginal;
    Character &round;
    char movement=' ';
    int maxSkillPoint;
    int skillPoint;
    int roundNumber;
    int difficulty;
    bool victory = false, timelineProceed = false, previous = false, reset = false;

    void forward(double time);
    Character &nextCharacter();
    bool incSkillPoint(int amount = 1);
    bool decSkillPoint(int amount = 1);
};

#endif //PROJECT_STATE_H
