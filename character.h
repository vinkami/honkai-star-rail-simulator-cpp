#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>
#include "state.h"

using namespace std;

class Character{
public:
    string name, role;
    int speed, hp, atk, def;
    int critRate, critDamage;
    double resetRemTime, remTime;

    Character(string name,string role, int speed, int hp, int atk, int def, int critRate, int critDamage);

    void forward(double time);
    void reset();

    void action(State &state);
    void print() const;
};

#endif //PROJECT_CHARACTER_H
