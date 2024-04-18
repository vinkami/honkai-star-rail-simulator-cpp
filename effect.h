#ifndef PROJECT_EFFECT_H
#define PROJECT_EFFECT_H

#include <string>
#include <functional>
#include "state.h"
using namespace std;

class Effect {
public:
    string name;
    int duration;

    function<void(State)> startRound{}, endRound{};

    Effect(string name, int duration);
};

#endif //PROJECT_EFFECT_H
