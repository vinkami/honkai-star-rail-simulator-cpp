//
// Created by vinkami on 11/4/2024.
//

#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>

using namespace std;

class Character{
public:
    string name;
    int speed;
    double resetRemTime;
    double remTime;

    Character(string name, int speed);

    void forward(double time);
    void reset();
//    string action(State state);  // TODO: figure out how to implement this without circular dependencies
};

#endif //PROJECT_CHARACTER_H
