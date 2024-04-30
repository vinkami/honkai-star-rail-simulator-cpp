#include "character.h"
#include "function.h"
#include <iostream>
#include <utility>
#include <algorithm>
#include <fstream>

using namespace std;

Character::Character(string name, int level, double speed, double hp, double atk, double def, double critRate, double critDamage, double maxEnergy, int taunt, double maxHp):
        name(std::move(name)), level(level),speed(speed), hp(hp), atk(atk), def(def), critRate(critRate), critDamage(critDamage), energy(50),
        baseSpeed(speed), baseHp(hp), baseAtk(atk), baseDef(def), baseCritRate(critRate), baseCritDamage(critDamage), maxEnergy(maxEnergy), taunt(taunt), maxHp(maxHp) {
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

void Character::printCurrentStatus() const {
    ifstream portraitFile("art/" + name + ".txt");
    if (portraitFile.is_open()) {
        cout << portraitFile.rdbuf();
        portraitFile.close();
    }
    cout << endl;
    if (faction == "ally") {
        slowPrint(name + ":\n", nameColor, 3);

        vector<int> color = {0};
        if (hp <= maxHp*0.3) color = {31};
        slowPrint("HP: " + to_string((int) hp) + "/" + to_string((int) maxHp) + "\n", color, 3);
        slowPrint("ATK: " + to_string((int) atk) + "\n", {0}, 3);
        slowPrint("DEF: " + to_string((int) def) + "\n", {0}, 3);
        slowPrint("Crit Rate: " + to_string((int) critRate) + "%\n", {0}, 3);
        slowPrint("Crit Damage: " + to_string((int) critDamage) + "%\n", {0}, 3);
        slowPrint("Speed: " + to_string((int) speed) + "\n", {0}, 3);
    } else {
        slowPrint("You are unable to see through the enemy completely\n", {31}, 3);
        slowPrint(name + ":\n", nameColor, 3);
        vector<int> color = {0};
        if (hp <= maxHp*0.3) color = {31};
        int healthPercent = (int) (hp/maxHp * 100);
        slowPrint("HP: " + to_string(healthPercent) + "%\n", color, 3);
    }
    if (!effects.empty()) {
        slowPrint("Effects: \n", {0}, 3);
        for (auto &efx: effects) {
            slowPrint("- " + efx.name + " with stack " + to_string(efx.stack), {0}, 3);
            if (efx.duration > 0) {
                slowPrint(" (" + to_string(efx.duration) + " turns left)\n", {0}, 0);
            } else {
                slowPrint(" (permanent)\n", {0}, 0);
            }
        }
    } else {
        slowPrint("No effects.\n", {0}, 3);
    }
    cout << endl;
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

//void Character::removeEffect(Effect &efx) {
//    auto it = find_if(effects.begin(), effects.end(), [efx](Effect &_efx) { return _efx.name == efx.name; });
//    if (it != effects.end()) effects.erase(it);
//    delete &efx;
//}
void Character::removeEffect(Effect& efx) {
    auto it = find_if(effects.begin(), effects.end(), [&efx](const Effect& _efx) { return _efx.name == efx.name; });
    if (it != effects.end()) {
        effects.erase(it);
    }
}

void Character::printColorName(int delayMS) {
    slowPrint(name, nameColor, delayMS);
}

void Character::cleanseDebuff() {
    for (auto &efx: effects) {
        if (efx.type == "debuff") {
            removeEffect(efx);
            slowPrint("cleansed debuff: "+efx.name+"\n",{92},0);
            return;
        }
    }
}

void Character::changeSpeed(double amount) {
    double remDist = remTime * speed;
    speed += amount;
    remTime = remDist / speed;
    resetRemTime = 10000.0 / speed;
}