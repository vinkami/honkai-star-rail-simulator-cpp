#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "function.h"
#include "state.h"
#include "character.h"
#include "logic.h"
#include "situation.h"

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

int searchCharacter(const vector<Character>& characters, const string& target) {
    for (int i = 0; i < characters.size(); ++i) {
        if (characters[i].name == target) {
            return i;
        }
    }
    return -1;
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
        cout << endl << "Select a target (1-" << characters.size() << " or {help|exit}, anything else defaults to 1): ";
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

// For character voice lines see https://wiki.biligame.com/sr/%E8%A7%92%E8%89%B2%E8%AF%AD%E9%9F%B3
// For character abilities see https://www.prydwen.gg/star-rail/characters
// If u can't 100% copy the ability, just make a similar one and write a comment to explain what changes u made
void insertCharacterAbility(Character &character) {
    if (character.name == "Clara") {
        Effect enhancedCounter = Effect("Enhanced Counter", -1, 0);
        character.effects.push_back(enhancedCounter);
        character.dmgReduction = 0.1;
        character.basicAtk = [](Character &self, State &state) {  // I Want to Help
            int target = selectTarget(state.enemies);
            Character &enemy = state.enemies[target];
            if (hit(50)) slowPrint("クラーラ：気を付けて、スヴァローグ！\n", {37});
            else slowPrint("スヴァローグ：排除する。\n", {36});
            singleAttack(state, self, target, 1.0);
            self.energy += 20;
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Svarog Watches Over You
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            if (hit(50)) slowPrint("スヴァローグ：隠れろ。\n", {36});
            else slowPrint("スヴァローグ：殲滅開始。\n", {36});
            for (auto &enemy : state.enemies) {
                if (enemy.getEffectLoc("Mark of Counter") != -1) {
                    enemy.effects.erase(enemy.effects.begin() + enemy.getEffectLoc("Mark of Counter"));
                    singleAttack(state, self, searchCharacter(state.enemies, enemy.name), 2.4);
                } else {
                    singleAttack(state, self, searchCharacter(state.enemies, enemy.name), 1.2);
                }
            }
            self.energy += 30;
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Promise, Not Command
            Effect enhancedCounter = self.getEffect("Enhanced Counter");
            slowPrint("クラーラ：クラーラも…みんなを守りたい！       \nクラーラ：助けて、スヴァローグ！\n", {37});
            self.energy += 5;
            Effect ultEfx("Promise, Not Command", 2, 0);
            ultEfx.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.dmgReduction -= 0.25;
                    master.removeEffect(self);
                }
            };
            self.dmgReduction += 0.25;
            enhancedCounter.stack += 2;
        };
        character.onHit = [](Character &self, State &state, Character &attacker) {
            Effect enhancedCounter = self.getEffect("Enhanced Counter");
            if (hit(30)) {  // should have determined by damage taken but I'm lazy
                slowPrint("クラーラ：…痛い。\n", {37});
            } else {
                slowPrint("スヴァローグ：平気か？\n", {36});
                slowPrint("クラーラ：大丈夫。\n", {37});
            }
            self.energy += 10;
            Effect counter("Mark of Counter", -1, 1);
            attacker.effects.push_back(counter);
            if (hit(50)) {
                slowPrint("スヴァローグ：クラーラから離れろ。\n", {36});
            } else {
                slowPrint("スヴァローグ：命令執行。\n", {36});
            }
            int target = searchCharacter(state.enemies, attacker.name);
            if (enhancedCounter.stack > 0) {
                blastAttack(state, self, target, 3.2, 1.6);
                enhancedCounter.stack--;
            } else {
                singleAttack(state, self, target, 1.6);
            }
        };
    }

    else if (character.name== "Jingliu") {
        Effect syzygy("Syzygy", -1, 0);
        Effect transmigration("Spectral Transmigration", -1, 0);
        character.effects.push_back(syzygy);
        character.effects.push_back(transmigration);

        character.basicAtk = [](Character &self, State &state) {  // Lucent Moonglow
            Effect &transmigration = self.getEffect("Spectral Transmigration");
            if (transmigration.stack == 1) {
                slowPrint("Basic Attack is disabled during Spectral Transmigration mode.\n",{91});
                return;
            }
            int target = selectTarget(state.enemies);
            slowPrint("鏡流：切先は戻らぬ！\n", {34});
            singleAttack(state, self, target, 1.0);
            self.energy += 20;
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {
            Effect &syzygy = self.getEffect("Syzygy");
            Effect &transmigration = self.getEffect("Spectral Transmigration");
            if (transmigration.stack == 0) {  // Transcendent Flash
                if (!state.decSkillPoint()) {
                    slowPrint("No skill points left.\n", {91});
                    return;
                }
                int target = selectTarget(state.enemies);
                slowPrint("鏡流：飛光よ、差せ！\n", {34});
                syzygy.stack += 1;
                slowPrint("Syzygy stack: " + to_string(syzygy.stack) + "\n",{34});
                singleAttack(state, self, target, 2.0);
                self.energy += 20;

                if (syzygy.stack >= 2) {
                    transmigration.stack = 1;
                    self.critRate += 50;
                    self.remTime = 0;
                    slowPrint("鏡流：乗月返真。 (Spectral Transmigration mode activated)\n", {34});
                }
            } if (transmigration.stack !=0 ) {  // Moon On Glacial River
                // does not consume skill point
                int target = selectTarget(state.enemies);
                slowPrint("鏡流：月光を剣とせん。\n",{34});
                // talent: drain allies' hp and increase atk
                double totalRemoved = 0;
                for (auto &ally : state.allies) {
                    if (&ally != &self) {
                        double removed = max(ally.baseHp * 0.04, 1.0);
                        totalRemoved += removed;
                        ally.hp -= removed;
                    }
                }
                double increasedAtk = max(totalRemoved * 5.4, self.baseAtk * 1.8);
                self.atk += increasedAtk;
                blastAttack(state, self, target, 2.5, 1.25);
                self.atk -= increasedAtk;
                self.energy += 30;
                syzygy.stack -= 1;
                if (syzygy.stack == 0) {
                    transmigration.stack = 0;
                    self.critRate -= 50;
                    slowPrint("Syzygy stack is 0. Special Transmigration mode deactivated.\n", {34});
                } else {
                    slowPrint("Syzygy stack: " + to_string(syzygy.stack) + " (Spectral Transmigration mode active)\n",{34});
                }
            }
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Florephemeral Dreamflux
            Effect &syzygy = self.getEffect("Syzygy");
            Effect &transmigration = self.getEffect("Spectral Transmigration");
            int target = selectTarget(state.enemies);
            slowPrint("鏡流：この月華で…       \n鏡流：すべてを照らさん！\n", {34});
            // talent: drain allies' hp and increase atk
            double totalRemoved = 0;
            for (auto &ally : state.allies) {
                if (&ally != &self) {
                    double removed = max(ally.baseHp * 0.04, 1.0);
                    totalRemoved += removed;
                    ally.hp -= removed;
                }
            }
            double increasedAtk = max(totalRemoved * 5.4, self.baseAtk * 1.8);
            self.atk += increasedAtk;
            blastAttack(state, self, target, 3.0, 1.5);
            self.atk -= increasedAtk;
            self.energy += 5;
            syzygy.stack += 1;
            slowPrint("Syzygy stack: " + to_string(syzygy.stack) + " (Spectral Transmigration mode active)\n",{34});
            if (syzygy.stack >= 2) {
                transmigration.stack = 1;
                self.critRate += 50;
                self.remTime = 0;
                slowPrint("鏡流：乗月返真。 (Spectral Transmigration mode activated)\n", {34});
            }
        };
    }
}
    // Todo: Add more characters

vector<Situation> getSituations() {
    vector<Situation> situations;
    ifstream situationFile("situations.txt"), enemyFile("enemies.csv");

    // setup situations without enemies
    string line, situationName, description;
    getline(situationFile, line);
    situationName = line.substr(2);  // first situation name
    while(getline(situationFile, line)) {
        if (line[0] == '-') {
            Situation situation(situationName, description);
            situations.push_back(situation);
            situationName = line.substr(2);
            description = "";
        } else {
            description += line + "\n";
        }
    }
    Situation situation(situationName, description);  // last situation
    situations.push_back(situation);
    situationFile.close();

    // fill in enemies
    getline(enemyFile,line);  // Skip the first line which explains the format
    while (getline(enemyFile, line)) {  // comma separated values
        stringstream ss(line);
        string name;
        int situationNo;
        double speed, hp, atk, def;
        char comma;

        getline(ss, name, ',');
        ss >> situationNo >> comma;
        ss >> speed >> comma;
        ss >> hp >> comma;
        ss >> atk >> comma;
        ss >> def;

        Character temp(name, speed, hp, atk, def, 0, 0, 0);
        temp.faction = "enemy";
        situations[situationNo].enemies.push_back(temp);
    }
    enemyFile.close();
    return situations;
}

vector<Character> getPlayableCharacters() {
    vector<Character> playableCharacters;
    ifstream characterFile("characters.csv");
    string line;
    getline(characterFile, line); // Skip the first line (header)
    while (getline(characterFile, line)) {  // comma separated values
        stringstream ss(line);
        string name;
        double speed, hp, atk, def, critRate, critDamage, maxEnergy;
        char comma;

        getline(ss, name, ',');
        ss >> speed >> comma;
        ss >> hp >> comma;
        ss >> atk >> comma;
        ss >> def >> comma;
        ss >> critRate >> comma;
        ss >> critDamage >> comma;
        ss >> maxEnergy;

        Character temp(name, speed, hp, atk, def, critRate, critDamage, maxEnergy);
        insertCharacterAbility(temp);
        playableCharacters.push_back(temp);
    }
    return playableCharacters;
}
