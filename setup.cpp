#include <iostream>
#include <sstream>
#include "setup.h"
#include "state.h"
#include "character.h"
#include "function.h"
#include "battle.h"
#include "situation.h"

using namespace std;

int searchCharacter(const vector<Character>& characters, const string& target) {
    for (int i = 0; i < characters.size(); ++i) {
        if (characters[i].name == target) {
            return i;
        }
    }
    return -1;
}

void teamMenu(State &state) {
    vector<Character> playableCharacters = getPlayableCharacters();
    cout << "Team Menu" << endl;
    vector<Character> team;
    cout << "Available Characters:" << endl;
    for (const auto &character : playableCharacters) character.print();
    cout << "A team must have 4 characters." << endl;
    cout << "Select a character to your team by typing their names. (Exact word)" << endl;
    while (team.size() != 4){
        string selection;
        cin >> selection;
        int temp = searchCharacter(playableCharacters, selection);
        if (temp > -1 && searchCharacter(team, selection) == -1) {
            team.push_back(playableCharacters[temp]);
            cout << selection << " has joined the team." << endl;
        } else if (searchCharacter(team, selection) != -1) {
            cout << "Character is already in team! Please choose again."<< endl;
        } else if (selection == "exit") {
            cout << "Character selection terminated." << endl;
            return;
        } else if (selection == "help") {
            printHelp("team");
        } else {
            cout << "Selected Character not found! Check whether there is a typo." << endl;
        }
    }
    cout << "Team formed successfully!" << endl << "Current Team:" << endl;
    for (const auto &character : team)
        cout << character.name << "     ";
    cout << endl;
    state.allies = team;
}

void battleMenu(State &state) {
    cout << "Battle Menu" << endl << endl;
    vector<Situation> situations = getSituations();
    for (int i = 0; i < situations.size(); i++) {
        cout << "Situation " << i+1 << endl;
        string description = situations[i].description;
        for (const auto& enemy : situations[i].enemies){
            cout << enemy.name << endl;
        }
        cout << endl;
    }
    // Todo: Add help and exit commands, process garbage input
    cout << "Please choose your situation (1/2/3)   ";
    int selection;
    cin >> selection;
 //   for (const auto& enemy: situations[selection-1]) enemy.print();
    state.enemies = situations[selection - 1].enemies;
    cout << "Situation selected successfully!" << endl;
    cout << "You will be facing " ;
    slowPrint(situations[selection - 1].name, {1,31}, 50);
    cout << endl;
}


void settingsMenu(State &state) {
    cout << "Settings Menu" << endl;
}

void debugMenu(State &state) {
    cout << "Debug Menu" << endl;
    cout << "Enter command: ";
    string input;
    while (getline(cin, input)) {
        stringstream ss(input);
        string cmd;
        ss >> cmd;
        if (cmd == "exit") {
            break;
        } else if (cmd == "start") {
            // teamMenu
            auto c = getPlayableCharacters();
            state.allies = {c[0], c[1], c[2], c[3]};
            cout << "Using " << c[0].name << ", " << c[1].name << ", " << c[2].name << ", " << c[3].name << " as team." << endl;

            // battleMenu
            Character dummy = Character("Dummy", 100, 100000, 0, 0, 0, 0);
            dummy.faction = "enemy";
            state.enemies = {dummy};
            cout << "Using " << dummy.name << " as enemy." << endl;

            // battle.cpp
            battleStart(state);
            while (gameLoop(state));
            battleEnd(state);
        } else {
            cout << "Unknown command" << endl;
        }
        cout << "Enter command: ";
    }
}