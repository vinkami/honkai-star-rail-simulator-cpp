//
// Created by vinkami on 19/4/2024.
//

#ifndef PROJECT_LOGIC_H
#define PROJECT_LOGIC_H

#include "character.h"

bool hit(double chance);
//double getNonCritDamage(double atk, double skillMultiplier, double def, double defIgnore=0);
//void attack(Character &attacker, Character &defender, double skillMultiplier);
void dot(Character &attacker, Character &defender, double skillMultiplier, const string& effect);
void singleHeal(State &state, Character &healer, int target, double skillMultiplier, double plus);
void blastHealing(State &state, Character &healer, int target, double mainSkillMultiplier, double adjacentSkillMultiplier,double mainPlus, double adjacentPlus );
void singleAttack(State &state, Character &attacker, int target, double skillMultiplier);
void blastAttack(State &state, Character &attacker, int target, double mainSkillMultiplier, double adjacentSkillMultiplier);
void aoeAttack(State &state, Character &attacker, double skillMultiplier);
void bounceAttack(State &state, Character &attacker, int target, double skillMultiplier, int bounceCount, int &hitCount);
#endif //PROJECT_LOGIC_H
