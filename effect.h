#ifndef PROJECT_EFFECT_H
#define PROJECT_EFFECT_H

#include <string>
#include <functional>

#include "state.h"
using namespace std;

class Effect {
private:
    function<void(Effect&, Character&, State&)> empty = [](Effect &self, Character &master, State &state) {};
public:
    string name;
    bool type;
    int duration, stack;
    vector<double> values{};

    function<void(Effect&, Character&, State&)> startRound = empty, endRound = empty;

    explicit Effect(string name, int duration = 1, int stack = 1, bool type= false);
};

#endif //PROJECT_EFFECT_H
