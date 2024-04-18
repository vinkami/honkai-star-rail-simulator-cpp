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
    State state(allies, enemies, round);

    slowPrint("答え、見つけてごらん？\n", 50, {38, 5, 160});
    slowPrint("Welcome to Honkai: Star Rail battle simulator.\nType 'help' for a list of commands.\nEnter command: ");
    string input;
    while (getline(cin, input)) {
        stringstream ss(input);
        string cmd;
        ss >> cmd;
        if (cmd == "exit") {
            break;
        } else if (cmd == "help") {
            printHelp("main"); //goes to function.cpp
        } else if (cmd == "start" && state.allies.size()==4) {
            battleStart(state); //goes to battle.cpp
            while (gameLoop(state)); //goes to battle.cpp
            battleEnd(state); //goes to battle.cpp
        } else if (cmd == "start" && state.allies.size() !=4) {
            cout << "Please form a team first!" << endl;
        } else if (cmd == "team") {
            teamMenu(state); //goes to setup.cpp
            getline(cin, input);
        } else if (cmd == "battle") {
            battleMenu(state); //goes to setup.cpp
        } else if (cmd == "settings") {
            settingsMenu(state); //goes to setup.cpp
        } else {
            cout << "Unknown command" << endl;
        }

        cout << "Enter command: ";
    }
    cout << "Goodbye!" << endl;
}
