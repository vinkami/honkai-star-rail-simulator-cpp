#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>
#include "state.h"

using namespace std;

class Character{
public:
    string name;
    int speed, hp, atk, def;
    int critRate, critDamage;
    double resetRemTime, remTime;

    Character(string name, int speed, int hp, int atk, int def, int critRate, int critDamage);

    void forward(double time);
    void reset();

    void action(State &state);
    void print() const;
};

#endif //PROJECT_CHARACTER_H
