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
    string name, type;  // type = {buff|debuff|other}
    int duration, stack;
    vector<double> values{};

    function<void(Effect&, Character&, State&)> startRound = empty, endRound = empty;

    explicit Effect(string name, string type, int duration = 1, int stack = 1);
};

#endif //PROJECT_EFFECT_H
