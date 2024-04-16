#include <iostream>
#include <fstream>
#include <sstream>
#include "setup.h"
#include "state.h"
#include "character.h"
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
    vector<Character> playableCharacter;
    cout << "Team Menu" << endl;
    vector<Character> allies;

//    // hardcode
//    Character jingliu("Jingliu", 96, 1435, 679, 485, 0, 0);
//    Character huohuo("Huohuo", 98, 1358, 601, 509, 0, 0);
//    Character kafka("Kafka", 100, 1086, 679, 485, 0, 0);
//    Character clara("Clara", 90, 1241, 737, 485, 0, 0);
//    playableCharacter.insert(playableCharacter.end(), {jingliu, huohuo, kafka, clara});

    // comma separated values
    ifstream characterFile("characters.csv");
    string line;
    getline(characterFile, line); // Skip the first line (header)
    while (getline(characterFile, line)) {
        stringstream ss(line);
        string name;
        int speed, hp, atk, def, critRate, critDamage;
        ss >> name >> speed >> hp >> atk >> def >> critRate >> critDamage;
        name = name.substr(0, name.size()-1);  // Remove the last comma
        Character temp(name, speed, hp, atk, def, critRate, critDamage);
        playableCharacter.push_back(temp);
        temp.print();
    }

    cout << "Select a character to your allies by typing their names. (Exact word)" << endl;
    while (allies.size()!=4){
        string selection;
        cin >> selection;
        int temp = searchCharacter(playableCharacter, selection);
        if (temp > -1) {
            allies.push_back(playableCharacter[temp]);
            cout << selection << " has joined the team." << endl;
        }
        else
            cout << "Selected Character not found! Check whether there is a typo." << endl;
    }
    cout << "Team formed successfully" << endl;
    for (const auto &character : allies)
        cout << character.name << endl;
    state.allies = allies;
    // TODO: Fix when going back to main(), cin detects something weird and prints "Enter command:Unknown command: "
}

void battleMenu(State &state) {
    cout << "Battle Menu" << endl;

}

void settingsMenu(State &state) {
    cout << "Settings Menu" << endl;
}

