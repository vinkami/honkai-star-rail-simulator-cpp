#include <iostream>
#include <sstream>
#include "battle.h"
#include "state.h"
#include "function.h"
#include "character.h"
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

    // actions
    if (move == "q") {
        next.basicAtk(state);
    } else if (move == "e") {
        next.skill( state);
    } else if (move == "1") {
        state.allies[0].ult(state);
    } else if (move == "2") {
        state.allies[1].ult(state);
    } else if (move == "3") {
        state.allies[2].ult(state);
    } else if (move == "4") {
        state.allies[3].ult(state);
    } else

    // other commands
    if (move == "escape") {
        cout << "You escaped. Battle terminated." << endl;
        return false;
    } else if (move == "exit") {
        cout << "Goodbye!" << endl;
        exit(0);
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

