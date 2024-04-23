#include <random>
#include <iostream>
#include "logic.h"
#include "character.h"
#include "function.h"

using namespace std;


/* Determine whether something hits in a random chance.
 * chance: err... the chance of something hits, in percents
 * return: whether it hits */
bool hit(double chance){    //simply it is crit rate www
    double min = 1;
    double max = 100;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(min, max); // distribution in range [1, 100)
    double randomNumber = dist(gen);
    return (randomNumber <= chance);
}

// refer to https://www.prydwen.gg/star-rail/guides/damage-formula/ for all formulas
double getNonCritDamage(double atk, double skillMultiplier, double def, double defIgnore, double dmgReduction, double dmgBonus) {
    double trueDef = def * (1 - defIgnore);
    if (trueDef < 0) trueDef = 0;
    double defMultiplier = 1 - trueDef / (trueDef + 1000);
    return atk * skillMultiplier * defMultiplier * (1 - dmgReduction) * (1 + dmgBonus);
}

void attack(Character &attacker, Character &defender, double skillMultiplier, State &state) {
    double damage = getNonCritDamage(attacker.atk, skillMultiplier, defender.def, attacker.defIgnore, attacker.dmgReduction, attacker.dmgBonus);
    bool crit = hit(attacker.critRate);
    if (crit) damage *= 1 + attacker.critDamage / 100;
    defender.hp -= damage;
    cout << attacker.name << " attacks " << defender.name << " for " << damage << " damage";
    if (crit) cout << " (critical)";
    cout << "." << endl;
    if (defender.hp <= 0) {
        attacker.energy += 10;
        slowPrint(defender.name + " is defeated!\n", {91});
    } else {
        defender.onHit(defender, state, attacker);
    }
}

void singleAttack(State &state, Character &attacker, int target, double skillMultiplier) {
    std::vector<Character>& team = (attacker.faction == "ally") ? state.enemies : state.allies;
    Character &defender = team[target];
    attack(attacker,defender,skillMultiplier, state);
    // add kafka follow up attack
}

void blastAttack(State &state, Character &attacker, int target, double mainSkillMultiplier, double adjacentSkillMultiplier) {
    std::vector<Character> &team = (attacker.faction == "ally") ? state.enemies : state.allies;
    if (target > 0) {
        Character &adjacent = team[target - 1];
        attack(attacker, adjacent, adjacentSkillMultiplier, state);
    }
    Character &defender = team[target];
    attack(attacker, defender, mainSkillMultiplier, state);
    if (target + 1 < team.size()) {
        Character &adjacent = team[target + 1];
        attack(attacker, adjacent, adjacentSkillMultiplier, state);
    }
}

void aoeAttack(State &state, Character &attacker, double skillMultiplier) {
    std::vector<Character>& team = (attacker.faction == "ally") ? state.enemies : state.allies;
    for (auto &defender : team)
    attack(attacker, defender, skillMultiplier, state);
}


void bounceAttack(State &state, Character &attacker, int target, double skillMultiplier, int bounceCount) {
    std::vector<Character>& team = (attacker.faction == "ally") ? state.enemies : state.allies;
    Character &defender = team[target];
    attack(attacker, defender, skillMultiplier, state);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, (int) team.size() - 1);
    while (bounceCount > 0) {
        int newTarget = dist(gen);
        Character &newDefender = team[newTarget];
        if (newDefender.hp > 0) {
            attack(attacker, newDefender, skillMultiplier, state);
            bounceCount--;
        }
    }
}