#ifndef PROJECT_EFFECT_H
#define PROJECT_EFFECT_H

#include <string>
#include <functional>
#include "state.h"
using namespace std;

class Effect {
private:
    function<void(State)> empty = [](State state) {};
public:
    string name;
    int duration, stack;

    function<void(State)> startRound = empty, endRound = empty;

    explicit Effect(string name, int duration = 1, int stack = 1);
};

#endif //PROJECT_EFFECT_H
