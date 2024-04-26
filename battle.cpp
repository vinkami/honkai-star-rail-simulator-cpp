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
        ally.maxHp = ally.hp;
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

void printCharacterQueue(State &state) {
    auto characters = state.allies;
    characters.insert(characters.end(),state.enemies.begin(), state.enemies.end());
    sort(characters.begin(), characters.end(), [](const Character &a, const Character &b) { return a.remTime < b.remTime; });
    slowPrint("\nNext in line ",{93}, 10);
    int repeatedTimes = 5;
    if (characters.size()<4) repeatedTimes = characters.size()+1;
    for (int i=1;i<(repeatedTimes);i++){
        slowPrint(" [ ", {0}, 10);
        slowPrint(characters[i].name, {characters[i].nameColor}, 10);
        slowPrint(" ] ", {0}, 10);
        if(i+1!=repeatedTimes)slowPrint("->",{93},10);
    }
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
        slowPrint(current.name, {current.nameColor}, 10);
        slowPrint("'s turn.\n", {31}, 10);
        current.basicAtk(current, state);

    } else { // player's turn
        slowPrint(current.name, {current.nameColor}, 10);
        slowPrint("'s turn.", {93}, 10);
//        cout << current.name << "'s turn" << endl;
        printCharacterQueue(state);
        cout << "\nSkill Points: "; //<< state.skillPoint << " / " << state.maxSkillPoint << endl;
        for (int i=0;i<state.maxSkillPoint;i++){
            if (i<state.skillPoint){cout << "■";}
            else cout << "□";
//            if(i!=state.maxSkillPoint-1) cout << " ";
        }
        cout << endl << "Energy: ";
        for (auto &ally: state.allies) {
            cout << ally.name << " " << ally.energy << " / " << ally.maxEnergy << "   ";
        }
        cout << endl << "Health: ";
        for (auto &ally: state.allies) {
            cout << ally.name << " " << ally.hp << " / " << ally.maxHp << "   ";
        }

        string line, move;
        cout << endl << "Action: ";
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
