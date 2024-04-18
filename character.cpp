#include "character.h"
#include <iostream>
#include <utility>

using namespace std;

Character::Character(string name, double speed, double hp, double atk, double def, double critRate, double critDamage):
    name(std::move(name)), speed(speed), hp(hp), atk(atk), def(def), critRate(critRate), critDamage(critDamage) {
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
    cout << "Name: " << name << endl;
    cout << "HP: " << hp << endl;
    cout << "ATK: " << atk << endl;
    cout << "DEF: " << def << endl;
    cout << "Speed: " << speed << endl;
    cout << "Crit: " << critRate << "% / " << critDamage << "%" << endl;
    cout << endl;
}