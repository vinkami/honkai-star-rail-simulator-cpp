//
// Created by vinkami on 11/4/2024.
//

#include "setup.h"
#include "state.h"
#include <iostream>
using namespace std;

void Character::printcharacter() const {
    cout << "Name: " << name << endl;
    cout << "HP: " << hp << endl;
    cout << "ATK: " << atk << endl;
    cout << "DEF: " << def << endl;
    cout << "Speed: " << speed << endl;
    cout << endl;
}

int targetinlist(const std::vector<Character>& characters, const std::string& target) {
    for (std::size_t i = 0; i < characters.size(); ++i) {
        if (characters[i].name == target) {
            return i;
        }
    }
    return -1;
}

vector<Character> teamMenu() {
    vector<Character> playableCharacter;
    cout << "Team Menu" << endl;
    vector<Character> allies;
    Character jingliu("jingliu", 96), huohuo("huohuo", 98), kafka("kafka", 100), clara("clara", 90);
    jingliu.hp = 1435;jingliu.atk = 679;jingliu.def = 485; playableCharacter.push_back(jingliu);
    huohuo.hp = 1358;huohuo.atk = 601;huohuo.def = 509; playableCharacter.push_back(huohuo);
    kafka.hp = 1086;kafka.atk = 679;kafka.def = 485;playableCharacter.push_back(kafka);
    clara.hp = 1241;clara.atk = 737;clara.def = 485;playableCharacter.push_back(clara);
    for (const auto & i : playableCharacter){
        i.printcharacter();
    }
    cout << "Select a character to your allies by typing their names. (Exact word)" << endl;
    while (allies.size()!=4){
        string selection;
        cin >> selection;
        int temp = targetinlist(playableCharacter, selection);
        if (temp > -1) {
            allies.push_back(playableCharacter[temp]);
            cout << allies[allies.size()].name  << " has joint the team." << endl;
        }
        else
            cout << "Selected Character not found! Check whether there is a typo." << endl;
    }
    cout << "Team formed successfully" << endl;
    for (const auto &character : allies)
        cout << character.name << endl;
    return allies;
}

void battleMenu(State &state) {
    cout << "Battle Menu" << endl;

}

void settingsMenu(State &state) {
    cout << "Settings Menu" << endl;
}

