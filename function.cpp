#include <iostream>
#include <fstream>
#include <sstream>
#include "function.h"
#include "state.h"
#include "character.h"
#include "logic.h"
using namespace std;

void printHelp(const string& name) {
    ifstream helpFile("help/" + name + ".txt");
    if (helpFile.is_open()) {
        cout << helpFile.rdbuf();
    } else {
        cout << "Help file not found." << endl;
    }
    helpFile.close();
}

/* Print text slowly, with a delay between each character
 * text: the text to print
 * delayMS: the delay between each character in milliseconds
 * sgr: a vector of SGR codes to apply to the text. See https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters */
void slowPrint(const string& text, const vector<int>& sgr, int delayMS) {
    // color code
    string code = "\033[";
    for (int i = 0; i < sgr.size(); ++i) {
        code += to_string(sgr[i]);
        if (i < sgr.size() - 1) {
            code += ";";
        }
    }
    code += "m";
    cout << code;

    // text
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMS));
    }
    cout << "\033[0m";
}


int selectTarget(vector<Character>& characters) {
    do {
        cout << "Available targets are: ";
        for (int i = 0; i < characters.size(); ++i) {
            cout << i + 1 << ". " << characters[i].name << " ";
        }
        cout << endl << "Select a move (1-" << characters.size() << " or {help|exit}, anything else defaults to 1): ";
        string line, move;
        getline(cin, line);
        stringstream ss(line);
        ss >> move;

        // process commands
        if (move == "help") {
            printHelp("battle");
            continue;
        } else if (move == "exit") {
            cout << "Goodbye!" << endl;
            exit(0);
        }

        // type cast to int, check if failed or out of range
        try {
            int target = stoi(move) - 1;
            if (target < 0 || target >= characters.size()) {
                cout << "Invalid target. Defaulting to 1" << endl;
                return 0;
            } else {
                return target;
            }
        } catch (invalid_argument& e) {
            cout << "Invalid target. Defaulting to 1" << endl;
            return 0;
        } catch (out_of_range& e) {
            cout << "Invalid target. Defaulting to 1" << endl;
            return 0;
        }
    } while (true);
}


void insertCharacterAbility(Character &character) {
    if (character.name == "Clara") {
        character.basicAtk = [](Character &self, State &state) {
            int target = selectTarget(state.enemies);
            Character &enemy = state.enemies[target];
            if (hit(50)) slowPrint("クラーラ：気を付けて、スヴァローグ！\n", {37});
            else slowPrint("スヴァローグ：排除する。\n", {36});
            attack(self, enemy, 1.0);
        };
        character.skill = [](Character &self, State &state) {
            if (hit(50)) slowPrint("スヴァローグ：隠れろ。\n", {36});
            else slowPrint("スヴァローグ：殲滅開始。\n", {36});
            for (auto &enemy : state.enemies) {
                attack(self, enemy, 1.2);
            }
        };
        character.ult = [](Character &self, State &state) {
            slowPrint("クラーラ：クラーラも…みんなを守りたい！       \nクラーラ：助けて、スヴァローグ！\n", {37});
            // Todo: Implement ultimate ability
        };
    }
    // Todo: Add more characters
}

vector<Character> getPlayableCharacters() {
    vector<Character> playableCharacters;
    ifstream characterFile("characters.csv");
    string line;
    getline(characterFile, line); // Skip the first line (header)
    while (getline(characterFile, line)) {  // comma separated values
        stringstream ss(line);
        string name;
        double speed, hp, atk, def, critRate, critDamage;
        char comma;

        getline(ss, name, ',');
        ss >> speed >> comma;
        ss >> hp >> comma;
        ss >> atk >> comma;
        ss >> def >> comma;
        ss >> critRate >> comma;
        ss >> critDamage;

        Character temp(name, speed, hp, atk, def, critRate, critDamage);
        insertCharacterAbility(temp);
        playableCharacters.push_back(temp);
    }

    return playableCharacters;
}