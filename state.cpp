#include "state.h"
#include "character.h"
using namespace std;

State::State(vector<Character> &allies, vector<Character> &enemies, Character &round): allies(allies), enemies(enemies), round(round) {
    maxSkillPoint = 5;
    skillPoint = 3;
    roundNumber = 0;
    victory = false;
}

void State::forward(double time) {
    for (auto & ally: allies) {
        ally.forward(time);
    }
    for (auto & enemy: enemies) {
        enemy.forward(time);
    }
    round.forward(time);
}

Character State::nextCharacter() {
    Character &next = round;
    for (auto & ally: allies) {
        if (ally.remTime < next.remTime && ally.hp > 0) {
            next = ally;
        }
    }
    for (auto & enemy: enemies) {
        if (enemy.remTime < next.remTime && enemy.hp > 0) {
            next = enemy;
        }
    }
    return next;
}
