#include <iostream>
#include <sstream>
#include "external/VariadicTable.h"
#include "setup.h"
#include "state.h"
#include "character.h"
#include "function.h"
#include "situation.h"

using namespace std;

//This function provides a list of playable characters with their skills and stats
void teamMenu(State &state) {
    vector<Character> playableCharacters = getPlayableCharacters();
    cout << "Team Menu" << endl;
    vector<Character> team;
    cout << "Available Characters:" << endl;
    for (int i = 1; i <= playableCharacters.size(); i++)
        cout << i << ". " << playableCharacters[i-1].name << endl;
//    for (const auto &character : playableCharacters) character.print();
    cout << "More details of character can be found in list." << endl;
    cout << "A team must have 4 characters." << endl;
    //Ask the user what characters they want to use
    cout << "Select a character to your team by typing their names. (1-4)" << endl;
    while (team.size() != 4){
        string selection;
        cin >> selection;
        if (selection != "exit" && selection != "help" && !all_of(selection.begin(), selection.end(), ::isdigit)) {
            cout << "Selected Character not found! Please input appropriate number!" << endl;
        } else if (selection == "exit") {
            cout << "Character selection terminated." << endl;
            return;
        } else if (selection == "help") {
            printHelp("team");
        } else {
            int characternumber = stoi(selection);
            if (characternumber > playableCharacters.size() || characternumber < 0){
                cout << "Please input appropriate number! " << endl;
            }
            else if (playableCharacters[characternumber-1].name != "Used"){
                team.push_back(playableCharacters[characternumber-1]);
                cout << playableCharacters[characternumber-1].name << " joined!" << endl;
                playableCharacters[characternumber-1].name = "Used";
            } else{
                cout << "Character already chosen. Please choose another! " << endl;
            }
        }
        cout << "Select a character to your team by typing their names. (1-4)" << endl;
    }
    cout << "Team formed successfully!" << endl << "Current Team:" << endl;
    for (const auto &character : team)
        cout << character.name << "     ";
    cout << endl << endl;
    state.alliesOriginal = team;
    state.allies = std::move(team);
}

void characterList(State &state){
    cout << "Character List" << endl;
    cout << "You can see character's abilities by typing number." << endl;
    vector<Character> playableCharacters = getPlayableCharacters();
    VariadicTable<int, string, int, double, double, double,double, double, double> vt({"number", "name", "level", "speed", "hp", "atk", "def", "critRate", "critDamage"}, 5);
    for (int i = 1; i <= playableCharacters.size(); i++ )
        vt.addRow(i,playableCharacters[i-1].name, playableCharacters[i-1].level, playableCharacters[i-1].speed, playableCharacters[i-1].hp, playableCharacters[i-1].atk, playableCharacters[i-1].def,playableCharacters[i-1].critRate,playableCharacters[i-1].critDamage);
    vt.print(std::cout);
    cout << "Enter number to see more details."<< endl;
    int number;
    cin >> number;
}

//Player can choose different situations with different enemies.
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
                    state.enemiesOriginal = situations[selection - 1].enemies;
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

//Player can choose difficulty of the game.
void settingsMenu(State &state) {
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

//Allows player to set character level.
void levelsetting(State &state){
    int level, position;
    string input;
    cout << "Command: ";
    while (getline(cin, input)) {
        stringstream ss(input);
        string cmd;
        ss >> cmd;
        if (cmd == "exit"){
            break;
        } else if (cmd == "help"){
            printHelp("level");
        } else if (cmd == "choose"){
            if (state.allies.empty()) {
                cout << "Please select a character first." << endl;
            }   else {
                cout << "Current statistic" << endl;
                VariadicTable<string, int, double, double, double,double, double, double> original({"name", "level", "speed", "hp", "atk", "def", "critRate", "critDamage"}, 5);
                for (const auto& character: state.allies)
                    original.addRow(character.name, character.level, character.speed, character.hp, character.atk, character.def,character.critRate,character.critDamage);
                original.print(std::cout);
                cout << "Choose a character(1-4): ";
                cin >> position;
                cout << "Choose level(1-80): ";
                cin >> level;
                state.allies[position - 1].level = level;
                int newhp =
                        (state.allies[position - 1].baseHp * 7.4 * level - state.allies[position - 1].baseHp * level -
                         state.allies[position - 1].baseHp * 7.4 + 80 * state.allies[position - 1].baseHp) / 79;
                int newatk =
                        (state.allies[position - 1].baseAtk * 7.4 * level - state.allies[position - 1].baseAtk * level -
                         state.allies[position - 1].baseAtk * 7.4 + 80 * state.allies[position - 1].baseAtk) / 79;
                int newdef =
                        (state.allies[position - 1].baseDef * 7.4 * level - state.allies[position - 1].baseDef * level -
                         state.allies[position - 1].baseDef * 7.4 + 80 * state.allies[position - 1].baseDef) / 79;
                state.allies[position - 1].hp = newhp;
                state.allies[position - 1].atk = newatk;
                state.allies[position - 1].def = newdef;
                VariadicTable<string, int, double, double, double,double, double, double> update({"name", "level", "speed", "hp", "atk", "def", "critRate", "critDamage"}, 5);
                for (const auto& character: state.allies)
                    update.addRow(character.name, character.level, character.speed, character.hp, character.atk, character.def,character.critRate,character.critDamage);
                update.print(std::cout);
            }
        }
        cout << "Command: " ;
    }
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
