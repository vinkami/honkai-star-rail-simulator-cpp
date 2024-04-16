#include "battle.h"
#include "state.h"
#include "function.h"
#include <iostream>
#include <random>
using namespace std;

void battleStart(State &state) {
    state.maxSkillPoint = 5;
    state.skillPoint = 3;
    state.roundNumber = 0;
}

bool gameLoop(State &state) {
    string move;
    cout << "Action:";
    cin >> move;

    if (move == "escape") {
        cout << "You escaped. Battle terminated." << endl;
        return false;
    } else if (move == "exit") {
        cout << "Goodbye!" << endl;
        exit(0);
    } else if (move == "help") {
        printHelp("battle");
    } else {
        cout << "Unknown command: " << move << endl;
    }

    return true;
}

void battleEnd(State &state) {

}

/* Determine whether an attack is a critical hit or not
 * critRate: the critical hit rate of the attacker
 * Return: true if the attack is a critical hit, false otherwise */
bool crit(int critrate){
    int min = 1;
    int max = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    int randomNumber = dist(gen);
    return (randomNumber<=critrate);
}