#include <random>
#include <iostream>
#include "logic.h"
#include "character.h"
using namespace std;


/* Determine whether something hits in a random chance.
 * chance: err... the chance of something hits, in percents
 * return: whether it hits */
bool hit(double chance){
    double min = 1;
    double max = 100;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(min, max); // distribution in range [1, 100)
    double randomNumber = dist(gen);
    return (randomNumber <= chance);
}

// refer to https://www.prydwen.gg/star-rail/guides/damage-formula/ for all formulas
double getNonCritDamage(double atk, double skillMultiplier, double def, double defIgnore) {
    double trueDef = def * (1 - defIgnore);
    if (trueDef < 0) trueDef = 0;
    double defMultiplier = 1 - trueDef / (trueDef + 1000);
    return atk * skillMultiplier * defMultiplier;
}

void attack(Character &attacker, Character &defender, double skillMultiplier) {
    double damage = getNonCritDamage(attacker.atk, skillMultiplier, defender.def, 0);
    bool crit = hit(attacker.critRate);
    if (crit) damage *= 1 + attacker.critDamage / 100;
    defender.hp -= damage;
    cout << attacker.name << " attacks " << defender.name << " for " << damage << " damage";
    if (crit) cout << " (critical)";
    cout << "." << endl;
    if (defender.hp <= 0) {
        cout << defender.name << " is defeated." << endl;
    }
}