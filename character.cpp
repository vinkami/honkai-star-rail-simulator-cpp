//
// Created by vinkami on 11/4/2024.
//

#include "character.h"

#include <utility>
using namespace std;

Character::Character(string name, int speed): name(std::move(name)), speed(speed) {
    this->resetRemTime = 10000.0 / speed;
    this->remTime = 15000.0 / speed;  // First round is longer
}

void Character::forward(double time) {
    remTime -= time;
    if (this->remTime < 0.0) this->remTime = 0.0;
}

void Character::reset() {
    remTime = resetRemTime;
}