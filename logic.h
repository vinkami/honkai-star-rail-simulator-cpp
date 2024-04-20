//
// Created by vinkami on 19/4/2024.
//

#ifndef PROJECT_LOGIC_H
#define PROJECT_LOGIC_H

#include "character.h"

bool hit(double chance);
//double getNonCritDamage(double atk, double skillMultiplier, double def, double defIgnore=0);
//void attack(Character &attacker, Character &defender, double skillMultiplier);

void singleAttack(State &state, Character &attacker, int target, double skillMultiplier);
void blastAttack(State &state, Character &attacker, int target, double mainSkillMultiplier, double adjacentSkillMultiplier);
void aoeAttack(State &state, Character &attacker, double skillMultiplier);
void bounceAttack(State &state, Character &attacker, int target, double skillMultiplier, int bounceCount);
#endif //PROJECT_LOGIC_H
