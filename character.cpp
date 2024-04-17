#include "character.h"
#include <iostream>
#include <utility>

using namespace std;

Character::Character(string name, string role, int speed, int hp, int atk, int def, int critRate, int critDamage):
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

void Character::action(State &state) {}  // No action by default.


void Character::print() const {
    cout << "Name: " << name << endl;
    cout << "Role: " << role << endl;
    cout << "HP: " << hp << endl;
    cout << "ATK: " << atk << endl;
    cout << "DEF: " << def << endl;
    cout << "Speed: " << speed << endl;
    cout << "Crit Ratio: " << critRate << " : " << critDamage << endl;
    cout << endl;
}