#include <iostream>
#include <sstream>
#include "VariadicTable.h"
#include "setup.h"
#include "state.h"
#include "character.h"
#include "function.h"
#include "situation.h"

using namespace std;


void teamMenu(State &state) {
    //This function provides a list of playable characters with their skills and stats
    vector<Character> playableCharacters = getPlayableCharacters();
    cout << "Team Menu" << endl;
    vector<Character> team;
    cout << "Available Characters:" << endl;
    VariadicTable<string, double, double, double,double, double, double> vt({"name", "speed", "hp", "atk", "def", "critRate", "critDamage"}, 5);
    for (const auto& character: playableCharacters)
        vt.addRow(character.name, character.speed, character.hp, character.atk, character.def,character.critRate,character.critDamage);
    vt.print(std::cout);
//    for (const auto &character : playableCharacters) character.print();
    cout << "A team must have 4 characters." << endl;
    //Ask the user what characters they want to use
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
    cout << endl << endl;
    state.allies = std::move(team);
}

void battleMenu(State &state) {
    //Player can choose different situations with different enemies.
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

    cout << "Please choose your situation (e.g. 1,2,3)  ";
    bool flag = true;
    while (flag) {
        string selectionStr;
        cin >> selectionStr;
        if (selectionStr == "help") {
            cout << endl;
            printHelp("enemy");
            cout << endl << "Please choose your situation (e.g. 1,2,3)  ";
        } else if (selectionStr == "exit") {
            cout << "Situation selection terminated." << endl;
            return;
        } else {
            try {
                int selection = std::stoi(selectionStr);
                if (selection >= 1 && selection <= situations.size()) {
                    state.enemies = std::move(situations[selection - 1].enemies);
                    cout << "Situation selected successfully!" << endl;
                    cout << "You will be facing ";
                    slowPrint(situations[selection - 1].name, {1,31}, 50);
                    cout << endl << endl;
                    flag = false;
                } else {
                    cout << "No situation found! Please try again." << endl;
                    cout << "Please choose your situation (e.g. 1,2,3)  ";
                }
            } catch (std::invalid_argument&) {
                cout << "Invalid input! Please try again." << endl;
                cout << "Please choose your situation (e.g. 1,2,3) or type 'help': ";
            }
        }
    }
}

void settingsMenu(State &state) {
    //Player can choose difficulty of the game.
    cout << "Settings Menu" << endl ;
    if (state.difficulty == 0.5){
        cout << "Current difficulty: Easy" << endl;
    }else if (state.difficulty == 1.0){
        cout << "Current difficulty: Medium" << endl;
    }else if (state.difficulty == 1.5){
        cout << "Current difficulty: Hard" << endl;
    }
    cout << "Select a difficulty for your game.(e.g. medium): ";
    string difficulty;
    bool flag = true;
    string resetColor="\033[0m";
    vector<int> color;
    while (flag){
        cin >> difficulty;
        cout << endl;
        if (difficulty=="help"){
            printHelp("setting");
            cout << endl << "Select a difficulty for your game.(e.g. medium) :   ";
        } else if (difficulty=="exit"){
            cout << "Difficulty selection terminated." << endl;
            return;
        } else if (difficulty=="easy" || difficulty=="Easy") {
            state.difficulty=0.5;
            color = {32};
            flag=false;
        } else if (difficulty=="medium" || difficulty=="Medium") {
            state.difficulty=1;
            color = {33};
            flag=false;
        } else if (difficulty=="hard" || difficulty=="Hard") {
            state.difficulty=1.5;
            color = {31};
            flag=false;
        } else{
            cout << "Invalid input! Please try again." << endl;
            cout << "Select a difficulty for your game.(e.g. medium) or type 'help' :   ";
        }
    }
//    cout << "Selected Difficulty is " << color << difficulty << resetColor << " mode" << endl << endl;
    slowPrint("You selected " + difficulty + " mode\n\n", color, 0);
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

        } else {
            cout << "Unknown command" << endl;
        }
        cout << "Enter command: ";
    }
}
