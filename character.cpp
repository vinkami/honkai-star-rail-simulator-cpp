#include "character.h"
#include "function.h"
#include <iostream>
#include <utility>
#include <algorithm>

using namespace std;

Character::Character(string name, int level, double speed, double hp, double atk, double def, double critRate, double critDamage, double maxEnergy, int taunt):
        name(std::move(name)), level(level),speed(speed), hp(hp), atk(atk), def(def), critRate(critRate), critDamage(critDamage), energy(50),
        baseSpeed(speed), baseHp(hp), baseAtk(atk), baseDef(def), baseCritRate(critRate), baseCritDamage(critDamage), maxEnergy(maxEnergy), taunt(taunt) {
    resetRemTime = 10000.0 / speed;
    remTime = 15000.0 / speed;  // First round is longer according to the original game
}

void Character::forward(double time) {
    remTime -= time;
    if (remTime < 0.0) remTime = 0.0;
}

void Character::reset() {
    remTime = resetRemTime;
}

void Character::print() const {
    //cout << "Name: " << name << endl;
    //cout << "Level: " << level << endl;
    //cout << "HP: " << hp << endl;
    //cout << "ATK: " << atk << endl;
    //cout << "DEF: " << def << endl;
    //cout << "Speed: " << speed << endl;
    //cout << "Crit: " << critRate << "% / " << critDamage << "%" << endl;
    //cout << endl;
}

int Character::getEffectLoc(const string &efxName) {
    for (int i = 0; i < effects.size(); i++) {
        if (effects[i].name == efxName) return i;
    }
    return -1;
}

Effect &Character::getEffectOrCrash(const string &efxName) {
    int loc = getEffectLoc(efxName);
    if (loc == -1) {
        cout << "Internal Error: effect not found." << endl;
    }
    return effects[loc];
}

void Character::removeEffect(Effect &efx) {
    auto it = find_if(effects.begin(), effects.end(), [efx](Effect &_efx) { return _efx.name == efx.name; });
    if (it != effects.end()) effects.erase(it);
    delete &efx;
}

void Character::printColorName(int delayMS) {
    slowPrint(name, nameColor, delayMS);
}