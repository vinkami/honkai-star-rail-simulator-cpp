#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "battle.h"
#include "character.h"
#include "state.h"
#include "setup.h"
#include "function.h"
#include "external/VariadicTable.h"

using namespace std;

int main() {
    vector<Character> allies {}, enemies {};
    vector<Character> alliesOriginal {}, enemiesOriginal {};
    Character round("Round", 0,100, 0, 0, 0, 0, 0, 0,0);
    round.faction = "round";
    State state(allies, enemies, round, alliesOriginal, enemiesOriginal);
    VariadicTable<double, double, double, double,  double,  double,  double,  double,  double> vt({"help", "start", "team", "list", "battle", "level", "settings", "restart", "quickstart"}, 9);

//    slowPrint("答え、見つけてごらん？\n", {38, 5, 160}, 50);
    slowPrint("Welcome to Honkai: Star Rail battle simulator.\nPlease choose one of the following commands, or type 'help' for details.\n");
    vt.print(cout);
    slowPrint("Enter command: ");
    string input;
    while (getline(cin, input)) {
        stringstream ss(input);
        string cmd;
        ss >> cmd;
        if (cmd == "exit") {
            break;
        } else if (cmd == "help") {
            printHelp("main");
            cout << endl;   // function.cpp
        } else if (cmd == "start") {
            if (state.allies.empty()) {
                cout << "Please form a team first!" << endl;
            } else if (state.enemies.empty()) {
                cout << "Please pick a situation to fight first!" << endl;
            } else {
                // battle.cpp
                state.previous=true;
                battleStart(state);
                while (gameLoop(state));
                battleEnd(state);
            }
        } else if (cmd == "team") {
            teamMenu(state);  // setup.cpp
            getline(cin, input);
        } else if (cmd == "list") {
            characterList(state);
            getline(cin, input);
        } else if (cmd == "battle") {
            battleMenu(state);  // setup.cpp
            getline(cin, input);
        } else if (cmd == "level"){
            levelsetting(state);
            getline(cin, input);
        } else if (cmd == "settings" || cmd == "setting") {
            settingsMenu(state); // setup.cpp
            getline(cin,input);
        } else if (cmd == "restart"){
            if (not state.previous){
                cout << "There is no previous match." << endl;
            } else {
                allies = alliesOriginal;
                enemies = enemiesOriginal;
                battleStart(state);
                while (gameLoop(state));
                battleEnd(state);
            }
        } else if (cmd == "quickstart") {
            // teamMenu
            auto c = getPlayableCharacters();
            state.allies = {c[0], c[1], c[2], c[3]};
            state.alliesOriginal = state.allies;
            cout << "Using " << c[0].name << ", " << c[1].name << ", " << c[2].name << ", " << c[3].name << " as team." << endl;

            // battleMenu
            Situation situation = getSituations()[0];
            state.enemies = situation.enemies;
            state.enemiesOriginal = state.enemies;
            cout << "Using " << situation.name << " as situation." << endl;

            // battle.cpp
            battleStart(state);
            while (gameLoop(state));
            battleEnd(state);
        } else {
            cout << "Unknown command" << endl;
        }
        cout << "Enter command: ";
    }
    cout << "Goodbye!" << endl;
}
