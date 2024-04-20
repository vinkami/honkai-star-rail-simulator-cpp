#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <string>
#include <functional>
#include "state.h"
#include "effect.h"

using namespace std;

class Character{
private:
    function<void(Character&, State&)> emptyTurnAbility = [](Character &self, State &state) { state.timelineProceed = true;};
    function<void(Character&, State&)> emptyAbility = [](Character &self, State &state) {};
public:
    string name, faction="ally";  // faction: ally / enemy / round
    double baseSpeed, baseHp, baseAtk, baseDef, baseCritRate, baseCritDamage, maxEnergy;
    double speed, hp, atk, def, critRate, critDamage, energy;
    double dmgReduction = 0.0, defIgnore = 0.0;
    double resetRemTime, remTime;

    vector<Effect> effects{};
    function<void(Character&, State&)>
        basicAtk = emptyTurnAbility,
        skill = emptyTurnAbility,
        ult = emptyAbility,
        startBattle = emptyAbility,
        startRound = [](Character &self, State &state) {for (auto &efx: self.effects) efx.startRound(efx, self, state);},
        endRound = [](Character &self, State &state) {for (auto &efx: self.effects) efx.endRound(efx, self, state);};
    function<void(Character&, State&, Character&)> onHit = [](Character &self, State &state, Character &attacker) {self.energy += 5;};


    Character(string name, double speed, double hp, double atk, double def, double critRate, double critDamage, double maxEnergy);

    void forward(double time);
    void reset();
    void print() const;
    int getEffectLoc(const string &efxName);
    Effect &getEffect(const string &efxName);  // NOTE: DO NOT USE WHEN YOU ARE NOT SURE THAT THE EFFECT EXISTS
    void removeEffect(Effect &efx);
};

#endif //PROJECT_CHARACTER_H
