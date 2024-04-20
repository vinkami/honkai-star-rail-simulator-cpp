#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>
#include <functional>
#include "state.h"
#include "effect.h"

using namespace std;

class Character{
private:
    function<void(Character &self, State &state)> emptyAbility = [](Character &self, State &state) {state.timelineProceed = true;};
public:
    string name, faction="ally";  // faction: ally / enemy / round
    double baseSpeed, baseHp, baseAtk, baseDef, baseCritRate, baseCritDamage, maxEnergy;
    double speed, hp, atk, def, critRate, critDamage, energy;
    double resetRemTime, remTime;

    vector<Effect> effects{};
    function<void(Character &self, State &state)>
        basicAtk = emptyAbility,
        skill = emptyAbility,
        ult = emptyAbility,
        onHit = [](Character &self, State &state) {self.energy += 5;},
        startBattle = emptyAbility,
        startRound = [](Character &self, State &state) {for (auto &efx: self.effects) efx.startRound(state);},
        endRound = [](Character &self, State &state) {for (auto &efx: self.effects) efx.endRound(state);};


    Character(string name, double speed, double hp, double atk, double def, double critRate, double critDamage, double maxEnergy);

    void forward(double time);
    void reset();
    void print() const;
    int getEffectLoc(const string &efxName);
    Effect &getEffect(const string &efxName);  // NOTE: DO NOT USE WHEN YOU ARE NOT SURE THAT THE EFFECT EXISTS
};

#endif //PROJECT_CHARACTER_H
