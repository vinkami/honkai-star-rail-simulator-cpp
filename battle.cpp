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

void tryUlt(int allyTarget, State &state) {
    string &allyName = state.alliesOriginal[allyTarget].name;
    int allyPos = searchCharacter(state.allies, allyName);
    if (allyPos == -1) {
        slowPrint(allyName + " has been defeated. Ultimate cannot be used!\n", {91});
        return;
    }
    Character &ally = state.allies[allyPos];
    if (ally.energy < ally.maxEnergy) {
        slowPrint("Not enough energy!\n", {91});
    } else {
        ally.ult(ally, state);
        ally.energy = 0;
    }
}

void battleStart(State &state) {
    state.maxSkillPoint = 5;
    state.skillPoint = 3;
    state.roundNumber = 1;
    for (auto &ally : state.allies) {
        ally.remTime = 15000.0 / ally.speed;  // First round is longer according to the original game
        ally.resetRemTime = 10000.0 / ally.speed;
        ally.energy = 50;  // Memory of chaos uses 50 energy at start
        ally.startBattle(ally, state);
    }
    for (auto &enemy : state.enemies) {
        enemy.remTime = 15000.0 / enemy.speed;
        enemy.resetRemTime = 10000.0 / enemy.speed;
        enemy.startBattle(enemy, state);
    }
    state.round.remTime = 150.0;  // speed for round is fixed at 100
    state.round.resetRemTime = 100.0;
    slowPrint("Round 1 starts\n", {1, 33}, 20);
}

bool gameLoop(State &state) {  // return value: whether the battle is still ongoing
    Character &current = state.nextCharacter();
    current.startRound(current, state);
    bool battleContinues = battleEndCheck(state);  // possible that the last character dies to damage over time or something and ends the battle
    if (!battleContinues) return false;

    if (current.faction == "round") {  // new round starts
        state.roundNumber++;
        slowPrint("Round " + to_string(state.roundNumber) + " starts\n", {1, 33}, 20);
        state.timelineProceed = true;

    } else if (current.faction == "enemy") {  // enemy's turn
        cout << current.name << "'s turn" << endl;
        current.basicAtk(current, state);

    } else { // player's turn
//        cout << current.name << "'s turn" << endl;
        slowPrint(current.name,{current.color},0);
        cout << "'s turn\nSkill Points: " << state.skillPoint << " / " << state.maxSkillPoint << endl;
        cout << "Energy: ";
        for (auto &ally: state.allies) {
            cout << ally.name << " " << ally.energy << " / " << ally.maxEnergy << "   ";
        }
        cout << endl;

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
            tryUlt(0, state);
        } else if (move == "2") {
            tryUlt(1, state);
        } else if (move == "3") {
            tryUlt(2, state);
        } else if (move == "4") {
            tryUlt(3, state);
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
        } else if (move == "reset") {
            state.reset = true;
            return false;
        } else {
            cout << "Unknown command" << endl;
        }
    }

    if (state.timelineProceed) {
        // remove dead characters
        for (int i=0;i<state.allies.size();i++){
            if (state.allies[i].hp<=0)
                state.allies.erase(next(state.allies.begin(), i));
        }
        for (int i=0;i<state.enemies.size();i++){
            if (state.enemies[i].hp<=0)
                state.enemies.erase(next(state.enemies.begin(), i));
        }

        // go to next character
        current.endRound(current, state);
        state.forward(current.remTime);
        current.reset();
        state.timelineProceed = false;
    }
    return battleEndCheck(state);
}

void battleEnd(State &state) {
    if (state.reset){
        cout << "Game reset" << endl;
    } else if (state.victory){
        slowPrint("Battle Over. Victory!\n", {1,34}, 20);
    } else {
        slowPrint("Battle Over. You lose. Imagine losing in a simulator.\n", {1,31}, 20);
    }
}
