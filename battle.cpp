#include <iostream>
#include <random>
#include <sstream>
#include "battle.h"
#include "state.h"
#include "function.h"
#include "character.h"
#include "skill.h"
using namespace std;

void battleStart(State &state) {
    state.maxSkillPoint = 5;
    state.skillPoint = 3;
    state.roundNumber = 0;
}



bool gameLoop(State &state) {
    Character next = state.nextCharacter();
    cout << next.name << "'s turn." << endl;

    string line, move;
    cout << "Action:";
    getline(cin, line);
    stringstream ss(line);
    ss >> move;
    int damage=0,heal=0;
    if (move == "escape") {
        cout << "You escaped. Battle terminated." << endl;
        return false;
    } else if (move == "exit") {
        cout << "Goodbye!" << endl;
        exit(0);
    } else if (move == "Q" || move == "q") {
        damage = normalAtk(next);

    } else if (next.role == "healer" && (move == "E" || move == "e")) {
        heal = skillHeal(next);
    } else if (move == "E" || move == "e") {
        damage = skillHeal()
    } else if (move == "help") {
        printHelp("battle");
    } else {
        cout << "Unknown command" << endl;
    }

//    Things to run after this character has finished their turn (put them in q and e commands):
//    state.forward(next.remTime);
//    next.reset();

    return true;
}

void battleEnd(State &state) {

}

