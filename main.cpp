#include <iostream>
#include <string>
#include <vector>
#include "battle.h"
#include "character.h"
#include "state.h"
using namespace std;

int main() {
    cout << "Welcome to Honkai: Star Rail battle simulator.\nType 'help' for a list of commands." << endl;
    string cmd;
    while (cin >> cmd) {
        if (cmd == "exit") {
            break;
        } else if (cmd == "help") {
            cout << "Commands: help, exit" << endl;
        } else if (cmd == "start") {
            vector<Character> allies {Character("A1", 133), Character("A2", 90), Character("A3", 170), Character("A4", 136)};
            vector<Character> enemies {Character("E1", 100), Character("E2", 200), Character("E3", 300), Character("E4", 400), Character("E5", 500)};
            State state(allies, enemies);

            battleStart(state);
            while(gameLoop(state));
            battleEnd(state);

            cout << "Battle ended." << endl;
        } else {
            cout << "Unknown command: " << cmd << endl;
        }
    }
    cout << "Goodbye!" << endl;
}
