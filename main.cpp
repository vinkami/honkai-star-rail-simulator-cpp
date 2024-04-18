#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "battle.h"
#include "character.h"
#include "state.h"
#include "setup.h"
#include "function.h"

using namespace std;

int main() {
    vector<Character> allies {};
    vector<Character> enemies {};
    Character round("Round", 100, 0, 0, 0, 0, 0);
    round.faction = "round";
    State state(allies, enemies, round);

//    slowPrint("答え、見つけてごらん？\n", {38, 5, 160}, 50);
    slowPrint("Welcome to Honkai: Star Rail battle simulator.\nType 'help' for a list of commands.\nEnter command: ");
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
                battleStart(state);
                while (gameLoop(state));
                battleEnd(state);
            }
        } else if (cmd == "team") {
            teamMenu(state);  // setup.cpp
            getline(cin, input);
        } else if (cmd == "battle") {
            battleMenu(state);  // setup.cpp
        } else if (cmd == "settings") {
            settingsMenu(state);  // setup.cpp
        } else if (cmd == "debug") {
            debugMenu(state);  // setup.cpp
        } else {
            cout << "Unknown command" << endl;
        }

        cout << "Enter command: ";
    }
    cout << "Goodbye!" << endl;
}
