#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>
#include <functional>
#include "state.h"

using namespace std;

class Character{
private:
    function<void(Character &self, State &state)> emptyAbility = [](Character &self, State &state) {};
    function<void(State &state)> emptyRoundAction = [](State &state) {};
public:
    string name, faction="ally";  // faction: ally / enemy / round
    double speed, hp, atk, def;
    double critRate, critDamage;
    double resetRemTime, remTime;
    int stack;

    function<void(Character &self, State &state)> basicAtk = emptyAbility, skill = emptyAbility, ult = emptyAbility;
    function<void(State &state)> startBattle = emptyRoundAction, startRound = emptyRoundAction, endRound = emptyRoundAction;


    Character(string name, double speed, double hp, double atk, double def, double critRate, double critDamage);

    void forward(double time);
    void reset();
    void print() const;
};

#endif //PROJECT_CHARACTER_H
