//
// Created by david on 4/18/2024.
//
#include "skill.h"
#include "character.h"
#include <random>

int normalAtk(const Character& attacker){
    int damage;
    if (attacker.role=="healer")
        damage=attacker.hp/2;
    else
        damage=attacker.atk;
    if (crit(attacker.critRate))
        damage*=(1+attacker.critDamage);
    return damage;
}

int skillAtk(const Character& attacker){
    return 0;
}

int skillHeal(const Character& attacker){
    return 0;
}

/* Determine whether an attack is a critical hit or not
 * critRate: the critical hit rate of the attacker
 * Return: true if the attack is a critical hit, false otherwise */
bool crit(int critRate){
    int min = 1;
    int max = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    int randomNumber = dist(gen);
    return (randomNumber<=critRate);
}