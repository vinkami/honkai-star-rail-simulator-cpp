#include <iostream>
#include <sstream>
#include <algorithm>
#include "battle.h"
#include "state.h"
#include "function.h"
#include "character.h"
using namespace std;

bool battleEndCheck(State &state) {
    if (none_of(state.allies.begin(), state.allies.end(), [](Character &character) { return character.hp > 0; })) {
        state.victory = false;
        return false;
    }
    if (none_of(state.enemies.begin(), state.enemies.end(), [](Character &character) { return character.hp > 0; })) {
        state.victory = true;
        return false;
    }
    return true;
}

void battleStart(State &state) {
    state.maxSkillPoint = 5;
    state.skillPoint = 3;
    state.roundNumber = 1;
    for (auto &ally : state.allies) {
        ally.remTime = 15000.0 / ally.speed;  // First round is longer according to the original game
        ally.resetRemTime = 10000.0 / ally.speed;
        ally.startBattle(state);
    }
    for (auto &enemy : state.enemies) {
        enemy.remTime = 15000.0 / enemy.speed;
        enemy.resetRemTime = 10000.0 / enemy.speed;
        enemy.startBattle(state);
    }
    state.round.remTime = 150.0;  // speed for round is fixed at 100
    state.round.resetRemTime = 100.0;
    slowPrint("Round 1 starts\n", {1, 33}, 20);
}

bool gameLoop(State &state) {  // return value: whether the battle is still ongoing
    Character &current = state.nextCharacter();
    current.startRound(state);
    bool battleContinues = battleEndCheck(state);  // possible that the last character dies to damage over time or something and ends the battle
    if (!battleContinues) return false;

    // new round starts
    if (current.faction == "round") {
        state.roundNumber++;
        slowPrint("Round " + to_string(state.roundNumber) + " starts\n", {1, 33}, 20);
        state.forward(current.remTime);
        current.reset();
        return true;
    }

    // enemy's turn
    if (current.faction == "enemy") {
        cout << current.name << "'s turn" << endl;
        current.basicAtk(current, state);
        current.endRound(state);
        state.forward(current.remTime);
        current.reset();
        return battleEndCheck(state);
    }

    // player's turn
    cout << current.name << "'s turn" << endl;
    cout << "Skill Points: " << state.skillPoint << " / " << state.maxSkillPoint << endl;

    string line, move;
    cout << "Action: ";
    getline(cin, line);
    stringstream ss(line);
    ss >> move;

    // actions
    if (move == "q") {
        current.basicAtk(current, state);
    } else if (move == "e") {
        current.skill(current, state);
    } else if (move == "1") {
        state.allies[0].ult(current, state);  // Note: ultimates always cut the queue and the timeline will not move forward
    } else if (move == "2") {
        state.allies[1].ult(current, state);
    } else if (move == "3") {
        state.allies[2].ult(current, state);
    } else if (move == "4") {
        state.allies[3].ult(current, state);
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

    if (state.timelineProceed) {
        // remove dead characters
        auto pred = [](Character &character) { return character.hp <= 0; };
        state.allies.erase(remove_if(state.allies.begin(), state.allies.end(), pred), state.allies.end());
        state.enemies.erase(remove_if(state.enemies.begin(), state.enemies.end(), pred), state.enemies.end());

        // go to next character
        current.endRound(state);
        state.forward(current.remTime);
        current.reset();
        state.timelineProceed = false;
    }

    return battleEndCheck(state);
}

void battleEnd(State &state) {
    if (state.victory){
        slowPrint("Battle Over. Victory!\n", {1,34}, 20);
    } else {
        slowPrint("Battle Over. You lose. Imagine losing in a simulator.\n", {1,31}, 20);
    }
}
