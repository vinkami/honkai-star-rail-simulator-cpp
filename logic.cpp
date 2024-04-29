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
// getNonCritDamage fuction is for calculating non-crit damage which target should take, also use for determind Dot damage in dot function

void dot(Effect &efx, Character &defender, double multiplier){
    double damage = getNonCritDamage(efx.values[0], multiplier, defender.def, 0.0, defender.dmgReduction, 0.0);
    defender.hp -= damage;
    slowPrint(defender.name + " received " + to_string((int) damage) + " DOT damage from " + efx.name + ".\n", {0});
    if (defender.hp <= 0) {
        addEnergy(*efx.applier, 10);
        slowPrint(defender.name + " is defeated!\n", {91});
    }
}
// dot function give dot damage on target and determind target deafeated or not
void heal(Character &healer, Character &target, double skillMultiplier, double plus){
    double healing = healer.baseHp*skillMultiplier+plus;
    double maxHP= target.baseHp;
    if (healing+target.hp>maxHP) target.hp=maxHP;
    else target.hp+=healing;
    int heals = static_cast<int>(healing);
    slowPrint(healer.name, {healer.nameColor}, 10);
    slowPrint((" heals "), {0}, 10);
    slowPrint(target.name, {target.nameColor}, 10);
    slowPrint(" for " , {0}, 10);
    slowPrint(to_string(heals),{0},10);
    slowPrint(" hp.\n",{0},10);
}
// heal fuction is use for healing the cahracter with calculating how many hp should be healed
// used in different healing function e.g. single healing blasthealing
void attack(Character &attacker, Character &defender, double skillMultiplier, State &state) {
    double damage = getNonCritDamage(attacker.atk, skillMultiplier, defender.def, attacker.defIgnore, attacker.dmgReduction, attacker.dmgBonus);
    bool crit = hit(attacker.critRate);
    if (crit) damage *= 1 + attacker.critDamage / 100;
    defender.hp -= damage;
    slowPrint(attacker.name, {attacker.nameColor}, 10);
    slowPrint((" attacks "), {0}, 10);
    slowPrint(defender.name, {defender.nameColor}, 10);
    slowPrint(" for ", {0}, 10);
    int integerDamage = static_cast<int>(damage);
    cout << integerDamage;
    slowPrint(" damage.",{0},10);
//    cout << attacker.name << " attacks " << defender.name << " for " << damage << " damage";
    if (crit) slowPrint(" (Critical)", {94});
    cout << endl;
    if (defender.hp <= 0) {
        attacker.energy += 10;
        slowPrint(defender.name + " is defeated!\n", {91});
    } else {
        defender.onHit(defender, state, attacker);
    }
}
// Attack function is one of the basic function for deducting hp to target
// the function calculate how much damage should be deducted from target and determind target death
// also being used in all attack function
void singleHeal(State &state, Character &healer, int target, double skillMultiplier, double plus){
    std::vector<Character>& team = (healer.faction == "ally") ? state.allies : state.enemies;
    Character &receiver =team[target] ;
    heal(healer,receiver,skillMultiplier,plus); //using heal function for healing
}
// singleheal fuction is healling a single characther by inputing the target position
void blastHealing(State &state, Character &healer, int target, double mainSkillMultiplier, double adjacentSkillMultiplier,double mainPlus, double adjacentPlus){
    std::vector<Character>& team = (healer.faction == "ally") ? state.allies : state.enemies;
    // checking is there an adjacent target on left side (position before target in state.enemies)
    if (target > 0) {
        Character &adjacent = team[target - 1];
        heal(healer, adjacent, adjacentSkillMultiplier, adjacentPlus);//if return true, heal adjacent target
    }
    // healing selected target (inputed)
    Character &receiver = team[target];
    heal(healer, receiver, mainSkillMultiplier, mainPlus);
    // checking is there an adjacent target on right side (position after target in state.enemies)
    if (target + 1 < team.size()) {
        Character &adjacent = team[target + 1];
        heal(healer, adjacent, adjacentSkillMultiplier, adjacentPlus); //if return true, heal adjacent target
    }
}


void singleAttack(State &state, Character &attacker, int target, double skillMultiplier) {
    std::vector<Character>& team = (attacker.faction == "ally") ? state.enemies : state.allies;
    Character &defender = team[target];
    attack(attacker,defender,skillMultiplier, state);// using attack function
    // add kafka follow up attack
}
// singleAttack function is use for attacking a single target
void blastAttack(State &state, Character &attacker, int target, double mainSkillMultiplier, double adjacentSkillMultiplier) {
    std::vector<Character> &team = (attacker.faction == "ally") ? state.enemies : state.allies;
    if (target > 0) { // determind left side
        Character &adjacent = team[target - 1];
        attack(attacker, adjacent, adjacentSkillMultiplier, state);
    }
    Character &defender = team[target];
    attack(attacker, defender, mainSkillMultiplier, state);// attacking origin selected target by user
    if (target + 1 < team.size()) { // determind right
        Character &adjacent = team[target + 1];
        attack(attacker, adjacent, adjacentSkillMultiplier, state);
    }
}
// blastAttack function is use for attacking up to 3 target which next to each other
void aoeAttack(State &state, Character &attacker, double skillMultiplier) {
    std::vector<Character>& team = (attacker.faction == "ally") ? state.enemies : state.allies; // determind which side to attack
    for (auto &defender : team)
    attack(attacker, defender, skillMultiplier, state);
}
// AoeAttack function will attack all target in opposite team i.e if attacker belong state.allies, then state.enemies
void search(int newTarget, vector<int> &unique){
    bool flag= true;
    for (const auto& element : unique){
        if (newTarget==element) flag= false; // if target position already in unique vector, flag become false and target wont input to unique
    }
    if (flag) unique.push_back(newTarget);
}
// search function is used in bounceAttack for counting hitted target
void bounceAttack(State &state, Character &attacker, int target, double skillMultiplier, int bounceCount,int &hitCount) {
    std::vector<Character>& team = (attacker.faction == "ally") ? state.enemies : state.allies;
    Character &defender = team[target];
    attack(attacker, defender, skillMultiplier, state);
    random_device rd;
    mt19937 gen(rd());
    vector<int> uniqueArray = {};
    uniform_int_distribution<int> dist(0, (int) team.size() - 1);
    while (bounceCount > 0) { // attackloop
        int newTarget = dist(gen); // finding new target using dist function
        search(newTarget,uniqueArray);
        Character &newDefender = team[newTarget];
        if (newDefender.hp > 0) {
            attack(attacker, newDefender, skillMultiplier, state);
            bounceCount--;
        }
    }
    hitCount=uniqueArray.size(); // give hitted target e.g. use for determind Asta team atk buff stack
}
// bounceAttack function will attack random character in target team