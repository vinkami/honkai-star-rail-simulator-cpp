#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>
#include <functional>
#include "state.h"

using namespace std;

class Character{
public:
    string name;
    double speed, hp, atk, def;
    double critRate, critDamage;
    double resetRemTime, remTime;

    function<void(State &state)> basicAtk{}, skill{}, ult{};

    Character(string name, double speed, double hp, double atk, double def, double critRate, double critDamage);

    void forward(double time);
    void reset();
    void print() const;
};

#endif //PROJECT_CHARACTER_H
