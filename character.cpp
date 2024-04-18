#include "character.h"
#include <iostream>
#include <utility>

using namespace std;

Character::Character(string name, double speed, double hp, double atk, double def, double critRate, double critDamage):
    name(std::move(name)), speed(speed), hp(hp), atk(atk), def(def), critRate(critRate), critDamage(critDamage) {
    this->resetRemTime = 10000.0 / speed;
    this->remTime = 15000.0 / speed;  // First round is longer according to the original game
}

void Character::forward(double time) {
    remTime -= time;
    if (this->remTime < 0.0) this->remTime = 0.0;
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