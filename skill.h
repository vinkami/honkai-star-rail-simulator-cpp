//
// Created by david on 4/17/2024.
//
#ifndef COMP2113_ENGG1340_GROUP39_SKILL_H
#define COMP2113_ENGG1340_GROUP39_SKILL_H
#include "state.h"

int skillAtk(const Character& attacker);
int skillHeal(const Character& attacker);
int normalAtk(const Character& attacker);
bool crit(const int critRate);
#endif //COMP2113_ENGG1340_GROUP39_SKILL_H
