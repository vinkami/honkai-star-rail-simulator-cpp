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

void checkCharacters(State &state) {
    // list out the names of all allies and enemies in the battle with order
    slowPrint("Allies:\n", {0}, 10);
    int i = 1;
    for (auto &ally : state.allies) {
        slowPrint(to_string(i) + ": " + ally.name + "\n", {ally.nameColor}, 10);
        i++;
    }
    slowPrint("Enemies:\n", {0}, 10);
    for (auto &enemy : state.enemies) {
        slowPrint(to_string(i) + ": " + enemy.name + "\n", {enemy.nameColor}, 10);
        i++;
    }

    slowPrint("Select the character to check (1-" + to_string(i-1) + " or exit, anything else return back to action): ", {0}, 10);
    string line, cmd;
    getline(cin, line);
    stringstream ss(line);
    ss >> cmd;
    if (cmd == "exit") {
        cout << "Goodbye!" << endl;
        exit(0);
    }
    try {
        int target = stoi(cmd);
        if (target >= 1 && target < i) {
            if (target <= state.allies.size()) {
                state.allies[target - 1].printCurrentStatus();
            } else {
                state.enemies[target - state.allies.size() - 1].printCurrentStatus();
            }
        } else {
            slowPrint("Returning to action.\n", {0}, 10);
            return;
        }
    } catch (invalid_argument &e) {
        slowPrint("Returning to action.\n", {0}, 10);
        return;
    } catch (out_of_range &e) {
        slowPrint("Returning to action.\n", {0}, 10);
        return;
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
        enemy.maxHp=static_cast<int>(enemy.hp);
        enemy.resetRemTime = 10000.0 / enemy.speed;
        enemy.startBattle(enemy, state);
    }
    state.round.remTime = 150.0;  // speed for round is fixed at 100
    state.round.resetRemTime = 100.0;
    slowPrint("----------Round 1 starts----------\n", {1, 33}, 20);
}

void printCharacterQueue(State &state) {
    auto characters = state.allies;
    characters.insert(characters.end(),state.enemies.begin(), state.enemies.end());
    sort(characters.begin(), characters.end(), [](const Character &a, const Character &b) { return a.remTime < b.remTime; });
    int listSize = 5;
    if(characters.size()<listSize) listSize = (int) characters.size();
    slowPrint("Next in line ",{93}, 10);
    for (int i=1;i<listSize;i++){
        slowPrint(" [ ", {0}, 10);
        slowPrint(characters[i].name, {characters[i].nameColor}, 10);
        slowPrint(" ] ", {0}, 10);
        if(i+1!=listSize)slowPrint("->",{93},10);
        else cout << endl;
    }
}

void printAllCharacters(State &state){
    auto characters = state.allies;
    characters.insert(characters.end(),state.enemies.begin(),state.enemies.end());
    for(int i=0;i<6;i++){
        for(int j=0;j<characters.size();j++){
            printCharacterByLine(characters[j].name,characters[j].nameColor[0],i);
        }
        cout << endl;
    }
}

bool gameLoop(State &state) {  // return value: whether the battle is still ongoing
    Character &current = state.nextCharacter();
    current.startRound(current, state);
    bool battleContinues = battleEndCheck(state);  // possible that the last character dies to damage over time or something and ends the battle
    if (!battleContinues) return false;
    checkDot(state,current);
    if (current.faction == "round") {  // new round starts
        state.roundNumber++;
        slowPrint("----------Round " + to_string(state.roundNumber) + " starts----------\n", {1, 33}, 20);
        state.timelineProceed = true;

    } else if (current.faction == "enemy") {  // enemy's turn
        slowPrint(current.name, {current.nameColor}, 10);
        slowPrint("'s turn.\n", {31}, 10);
        current.basicAtk(current, state);

    } else { // player's turn
//        cout << current.name << "'s turn" << endl;
        printCharacterQueue(state);

        printAllCharacters(state);
//        printCharacterPortrait(current.name);
//        printCharacterByLine(current.name,2);
//        printCharacterByLine(current.name,3);
//        printCharacterByLine(current.name,4);
//        printCharacterByLine(current.name, 5);
        cout << "Energy: ";
//        for (auto ally: state.allies) {
//            string color = "\033[0m";
//            if (ally.maxEnergy==ally.energy)
//                color = "\033["+to_string(ally.nameColor[0])+"m";
//            cout << color << ally.name << " " << ally.energy << " / " << ally.maxEnergy << "\033[0m   ";
//        }

        // Print energy of all allies
        for (const auto& ally: state.allies) {
            auto color = ally.nameColor;
            if (ally.energy < ally.maxEnergy) color = {0};
            slowPrint(ally.name + " " + to_string((int) ally.energy) + " / " + to_string((int) ally.maxEnergy) + "   ",
                      {ally.nameColor}, 0);
        }
        cout << endl;

        // Print health of all allies
        cout << "Allie's Health: ";
        for (const auto &ally: state.allies) {
//            int health = static_cast<int>(ally.hp);
//            string color = "\033[0m";
//            if (ally.hp <= ally.maxHp*0.3)
//                color = "\033[31m";
//            cout << ally.name << " " << color << health << " / " << ally.maxHp << "\033[0m   ";
            vector<int> color = {0};
            if (ally.hp <= ally.maxHp*0.3) color = {31};
            slowPrint(ally.name + " " + to_string((int) ally.hp) + " / " + to_string((int) ally.maxHp) + "   ", {ally.nameColor}, 0);
        }
        cout << endl;

        // Print health of all enemies
        cout << "Enemy's Health: ";
        for (auto &enemy: state.enemies) {
//            string color = "\033[0m";
//            double health = enemy.hp/enemy.maxHp * 100;
//            int healthPercent = static_cast<int>(health);
//            if (health <= 30)
//                color = "\033[31m";
//            cout<< enemy.name << " " << color << healthPercent << "%\033[0m   ";
            vector<int> color = {0};
            if (enemy.hp <= enemy.maxHp*0.3) color = {31};
            int healthPercent = (int) (enemy.hp/enemy.maxHp * 100);
            slowPrint(enemy.name + " " + to_string(healthPercent) + "%   ", {enemy.nameColor}, 0);
        }
        cout << endl << endl;

        slowPrint(current.name, {current.nameColor}, 10);
        slowPrint("'s turn.          ", {93}, 10);
        slowPrint("Skill Points: ", {0}, 10); //<< state.skillPoint << " / " << state.maxSkillPoint << endl;
        for (int i=0; i<state.maxSkillPoint; i++){
            if (i<state.skillPoint) cout << "■";
            else cout << "□";
//            if(i!=state.maxSkillPoint-1) cout << " ";
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
        } else if (move == "check") {
            checkCharacters(state);
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
