#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include "function.h"
#include "state.h"
#include "character.h"
#include "logic.h"
#include "situation.h"

using namespace std;

//Show the help list in different setting.
void printHelp(const string& name) {
    ifstream helpFile("help/" + name + ".txt");
    if (helpFile.is_open()) {
        cout << helpFile.rdbuf();
    } else {
        cout << "Help file not found." << endl;
    }
    helpFile.close();
}

void printDescription(const string &name){
    ifstream description("Description/" + name + "des.txt");
    if (description.is_open()){
        cout << description.rdbuf();
    } else{
        cout << "Character not found." << endl;
    }
    description.close();
}

void printCharacterPortrait(const string &name) {
    ifstream portraitFile("ANSI/" + name + ".txt");
    if (portraitFile.is_open()) {
        string l;
        while (getline(portraitFile, l)) {
            for (int i = 0; i < l.size(); i++) {
                if (l[i] == '\\' && l[i + 1] == 'e') {
                    l[i] = '\033';
                    l.erase(i + 1, 1);
                }
            }
            cout << l << endl;
        }
    } else {
        cout << "Portrait file not found." << endl;
    }
    portraitFile.close();
}


void printMainCommandSelected(int num) {
    vector<string> list = {"help","start", "team", "list", "battle", "level", "settings", "restart", "quickstart"};
    for (int i=0;i<9;i++){
        if (num == i) slowPrint("["+list[i]+"]", {31}, 0);
        else cout << " " << list[i] << " ";
        if (i !=8) cout << " | ";
        else cout << endl;
    }
}

//To find whether character is available.
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

void addEnergy(Character &target, double increase){
    if (target.energy+increase<target.maxEnergy)
        target.energy+=increase;
    else target.energy=target.maxEnergy;
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

int aiTarget(vector<Character>& character){
    int total=0,temp=0;
    for (const auto& characters : character)
        total+=characters.taunt;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, total- 1);
    int randomNumber = distribution(gen);
    for (int i=0;i<character.size();i++){
        temp+=character[i].taunt;
        if (temp>randomNumber)
            return i;
    }
    return -1;
}


void insertEnemyAbility(Character &enemy){
    if (enemy.name== "Antibaryon") {
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            slowPrint("Antibaryon: 消滅\n",{31});
            singleAttack(state, self, target, 2.5);
            state.timelineProceed = true;
        };
    } else if (enemy.name== "Trampler"){
        enemy.basicAtk = [](Character &self, State &state){
            Effect lockOn("lockOn", "debuff", -1, 1);  // idk what happen
            int target= aiTarget(state.allies);
            bool locked= false;
            for (int i=0;i<state.allies.size();i++){
                for (const auto &effect : state.allies[i].effects){
                    if (effect.name=="lockOn"){
                        target=i;
                        locked= true;
                    }
                }
            }
            if (locked) {
                slowPrint("Trampler: 強弓の終末\n");
                singleAttack(state, self, target, 6.0);
                state.allies[target].removeEffect(lockOn);
            } else {
                if (hit(20)) {
                    slowPrint("Trampler: 螺旋の弓矢\nTrampler locks on to "+state.allies[target].name+"!\n"+"Cast End of Bow to this target in the next action.\n",{37});
                    state.allies[target].effects.push_back(lockOn);
                } else if (hit(37.5)){
                    slowPrint("Trampler: 侵略の鉄蹄\n",{37});
                    singleAttack(state,self,target,4.0);
                } else if (hit(60)){
                    slowPrint("Trampler: 戦争の蹂躙\n",{37});
                    blastAttack(state,self,target,2.5,2.5);
                }else {
                    slowPrint("Trampler: 虚実の投影\n",{37});
                    singleAttack(state,self,target,3.0);
                }
            }
            state.timelineProceed = true;
        };
    } else if (enemy.name== "Reaver"){
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            if (hit(50)) {
                slowPrint("Reaver: 略奪の斬撃\n", {31});
                singleAttack(state, self, target, 2.5);
            } else {
                slowPrint("Reaver: 狩猟の刃\n",{31});
                blastAttack(state,self,target,1.5,1.5);
            }
            state.timelineProceed = true;
        };
        // https://honkai-star-rail.fandom.com/wiki/Automaton_Spider
        // AutomationSpider, 1, 100, 10000, 552, 900, 500
    } else if (enemy.name == "AutomatonSpider") {
        enemy.basicAtk = [](Character &self, State &state) {
            //enemy.nameColor = {34}; todo: IDK why there is bug in this line
            int target= aiTarget(state.allies);
            if (hit(50)) {
                slowPrint("AutomatonSpider: 突進！\n",self.nameColor);
                singleAttack(state, self, target, 2.0);
            } else {
                slowPrint("AutomatonSpider: 時限モジュール!!!\nThe next action uses \"自爆モジュール\"\n");
                //idk is there any lyris so i put in megumin lyris from konosuba
                Effect explosion = Effect("Self-destruct", "other", 1, 0);
                self.effects.push_back(explosion);
                explosion.endRound = [](Effect &self, Character &master, State &state) {
                    self.duration--;
                    if (self.duration == 0) {
                        int target = aiTarget(state.allies);
                        blastAttack(state,master,target,5.0,5.0);
                        master.hp = 0;
                    }
                };
            }
        };

    } else if (enemy.name == " ") {

    }
}


// For character voice lines see https://wiki.biligame.com/sr/%E8%A7%92%E8%89%B2%E8%AF%AD%E9%9F%B3
// For character abilities see https://www.prydwen.gg/star-rail/characters
// If u can't 100% copy the ability, just make a similar one and write a comment to explain what changes u made
void insertCharacterAbility(Character &character) {
    if (character.name == "Clara") {
        //        character.name= "\033[91mClara\033[0m";
        character.nameColor = {36};
        Effect enhancedCounter = Effect("Enhanced Counter", "other", -1, 0);
        character.effects.push_back(enhancedCounter);
        character.dmgReduction = 0.1;
        character.basicAtk = [](Character &self, State &state) {  // I Want to Help
            int target = selectTarget(state.enemies);
            if (hit(50)) slowPrint("クラーラ：気を付けて、スヴァローグ！\n", self.nameColor);
            else slowPrint("スヴァローグ：排除する。\n", self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self,20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Svarog Watches Over You
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            if (hit(50)) slowPrint("スヴァローグ：隠れろ。\n", self.nameColor);
            else slowPrint("スヴァローグ：殲滅開始。\n", self.nameColor);
            for (auto &enemy : state.enemies) {
                if (enemy.getEffectLoc("Mark of Counter") != -1) {
                    enemy.effects.erase(enemy.effects.begin() + enemy.getEffectLoc("Mark of Counter"));
                    singleAttack(state, self, searchCharacter(state.enemies, enemy.name), 2.4);
                } else {
                    singleAttack(state, self, searchCharacter(state.enemies, enemy.name), 1.2);
                }
            }
            addEnergy(self,30);
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Promise, Not Command
            Effect enhancedCounter = self.getEffectOrCrash("Enhanced Counter");
            self.taunt*=5;
            slowPrint("クラーラ：クラーラも…みんなを守りたい！       \nクラーラ：助けて、スヴァローグ！\n", {37});
            addEnergy(self,5);
            Effect ultEfx("Promise, Not Command", "buff", 2, 0);
            ultEfx.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.taunt/=5;
                    master.dmgReduction -= 0.25;
                    master.removeEffect(self);
                }
            };
            self.dmgReduction += 0.25;
            enhancedCounter.stack += 2;
        };
        character.onHit = [](Character &self, State &state, Character &attacker) {  // Because We're Family
            Effect enhancedCounter = self.getEffectOrCrash("Enhanced Counter");
            if (hit(30)) {  // should have determined by damage taken but I'm lazy
                slowPrint("クラーラ：…痛い。\n", {37});
            } else {
                slowPrint("スヴァローグ：平気か？\n", {36});
                slowPrint("クラーラ：大丈夫。\n", {37});
            }
            addEnergy(self,10);
            Effect counter("Mark of Counter", "other", -1, 1);
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
        //        character.name="\033[34mJingliu\033[0m";
        character.nameColor = {34};
        Effect syzygy("Syzygy", "other" -1, 0);
        Effect transmigration("Spectral Transmigration", "other", -1, 0);
        character.effects.push_back(syzygy);
        character.effects.push_back(transmigration);

        character.basicAtk = [](Character &self, State &state) {  // Lucent Moonglow
            Effect &transmigration = self.getEffectOrCrash("Spectral Transmigration");
            if (transmigration.stack == 1) {
                slowPrint("Basic Attack is disabled during Spectral Transmigration mode.\n",{91});
                return;
            }
            int target = selectTarget(state.enemies);
            slowPrint("鏡流：切先は戻らぬ！\n", self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self,20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {
            Effect &syzygy = self.getEffectOrCrash("Syzygy");
            Effect &transmigration = self.getEffectOrCrash("Spectral Transmigration");
            if (transmigration.stack == 0) {  // Transcendent Flash
                if (!state.decSkillPoint()) {
                    slowPrint("No skill points left.\n", {91});
                    return;
                }
                int target = selectTarget(state.enemies);
                slowPrint("鏡流：飛光よ、差せ！\n", self.nameColor);
                syzygy.stack += 1;
                slowPrint("Syzygy stack: " + to_string(syzygy.stack) + "\n",self.nameColor);
                singleAttack(state, self, target, 2.0);
                addEnergy(self,20);

                if (syzygy.stack >= 2) {
                    transmigration.stack = 1;
                    self.critRate += 50;
                    self.remTime = 0;
                    slowPrint("鏡流：乗月返真。 (Spectral Transmigration mode activated)\n", self.nameColor);
                }
            } if (transmigration.stack !=0 ) {  // Moon On Glacial River
                // does not consume skill point
                int target = selectTarget(state.enemies);
                slowPrint("鏡流：月光を剣とせん。\n",self.nameColor);
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
                addEnergy(self,30);
                syzygy.stack -= 1;
                if (syzygy.stack == 0) {
                    transmigration.stack = 0;
                    self.critRate -= 50;
                    slowPrint("Syzygy stack is 0. Special Transmigration mode deactivated.\n", self.nameColor);
                } else {
                    slowPrint("Syzygy stack: " + to_string(syzygy.stack) + " (Spectral Transmigration mode active)\n", self.nameColor);
                }
            }
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Florephemeral Dreamflux
            Effect &syzygy = self.getEffectOrCrash("Syzygy");
            Effect &transmigration = self.getEffectOrCrash("Spectral Transmigration");
            int target = selectTarget(state.enemies);
            slowPrint("鏡流：この月華で…       \n鏡流：すべてを照らさん！\n",  self.nameColor);
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
            addEnergy(self,5);
            syzygy.stack += 1;
            slowPrint("Syzygy stack: " + to_string(syzygy.stack) + "\n", self.nameColor);
            if (syzygy.stack >= 2) {
                transmigration.stack = 1;
                self.critRate += 50;
                self.remTime = 0;
                slowPrint("鏡流：乗月返真。 (Spectral Transmigration mode activated)\n",  self.nameColor);
            }
        };
    }
    else if (character.name == "Tingyun") {
        //        character.name= "\033[91mClara\033[0m";
        //Effect enhancedCounter = Effect("Enhanced Counter", -1, 0);
        character.nameColor ={95};
        character.basicAtk = [](Character &self, State &state) {  // Dislodged
            Effect &transmigration = self.getEffectOrCrash("Spectral Transmigration");
            if (transmigration.stack == 1) {
                slowPrint("Basic Attack is disabled during Spectral Transmigration mode.\n",self.nameColor);
                return;
            }
            int target = selectTarget(state.enemies);
            slowPrint("落ち着いて。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self,20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Soothing Melody
            Effect benediction("Benediction", "buff", 3, 0);  // benediction for allies
            Effect speed_up ("Speed up", "buff", 1, 0);  // self speed up
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            // benediction part
            slowPrint("停云:万事順調～\n停云：諸悪退散～\n",  self.nameColor);
            int target = selectTarget(state.allies);// selecting target
            Character &allies = state.allies[target];
            double increased_atk = allies.baseAtk * 0.5;
            if(increased_atk > self.atk*0.25) {
                increased_atk = self.atk*0.25;
            }
            benediction.values.push_back(increased_atk);
            allies.getEffectOrCrash("Benediction");
            allies.atk += increased_atk;
            benediction.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.atk-=self.values[0];
                    master.removeEffect(self);
                }
            };
            // benediction part end
            // speed up
            self.getEffectOrCrash("Speed up");
            double increased_speed = self.baseSpeed*0.2;
            self.speed += increased_speed;
            speed_up.values.push_back((increased_speed));
            speed_up.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.speed -= self.values[0];
                    master.removeEffect((self));
                }
            };
        };
        character.ult = [](Character &self, State &state) {  // Amidst the Rejoicing Clouds
            int target = selectTarget(state.allies);
            Character &allies = state.allies[target];
            slowPrint("停云:万事吉とならん、一心同帰。      \n",  self.nameColor);
            addEnergy(allies,50);
            Effect ultEfx("Amidst the Rejoicing Clouds", "buff", 2, 0);
            allies.effects.push_back(ultEfx);
            double dmg_b = 0.5;
            allies.dmgBonus += dmg_b;
            ultEfx.values.push_back(dmg_b);
            ultEfx.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.dmgBonus -= self.values[0];
                    master.removeEffect((self));
                }
            };
            // Todo: Add more characters
        };
    }
    else if (character.name == "Kafka") {
        Effect elecshock = Effect("電觸", "debuff", 2 ,0);
        elecshock.endRound = [](Effect &self, Character &master, State &state) {
            self.duration--;
            //singleAttack(state, )
        };
        character.nameColor ={35};
        character.basicAtk = [](Character &self, State &state) {  // Midnight Tumul
            int target = selectTarget(state.enemies);
            slowPrint("一瞬よ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self,20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        //Todo: follow up attack
        character.skill = [](Character &self, State &state) {  // Caressing Moonlight
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            int target = selectTarget(state.enemies);
            singleAttack(state, self, target, 2.0);
            blastAttack(state, self, target, 2.0, 0.75);
            //slowPrint((" ")); seem there are no lyris when kafka use skill
            addEnergy(self,30);
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Twilight Trill
            aoeAttack(state, self, 0.8);
            // Todo: where is the remaining ult?
        };
    } else if (character.name=="Huohuo") {
        character.nameColor = {32};
        character.basicAtk = [](Character &self, State &state) {  // Banner: Stormcaller
            int target = selectTarget(state.enemies);
            if (hit(50)) slowPrint("シッポ：ほーらよっと。\nフォフォ：やあああ――\n", self.nameColor);
            else slowPrint("フォフォ：いやあ助けてええっ――\n", self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Talisman: Protection
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            int targetPos = selectTarget(state.allies);
            Character &target = state.allies[targetPos];
            if (hit(50)) slowPrint("フォフォ：邪を払い...魅を縛らん...\n", self.nameColor);
            else slowPrint("フォフォ：霊符よ...守りたまえ...\n", self.nameColor);
            addEnergy(self,30);
            target.cleanseDebuff();
            blastHealing(state, self, targetPos, 0.21, 0.168, 560, 448);
            // divine provision - added to allies instead for simplicity
            for (auto &ally: state.allies) {
                Effect skillEfx("Divine Provision", "buff", 2, 1);
                skillEfx.startRound = [](Effect &self, Character &master, State &state) {
                    if (master.name == "Huohuo") {
                        self.duration--;
                        if (self.duration == 0) {
                            for (auto &ally: state.allies) {
                                ally.removeEffect(ally.getEffectOrCrash("Divine Provision"));
                            }
                        }
                    }
                    // heal
                    int huohuoPos = searchCharacter(state.allies, "Huohuo");
                    if (huohuoPos != -1) {
                        for (auto &ally: state.allies) {
                            ally.removeEffect(ally.getEffectOrCrash("Divine Provision"));
                        }
                    }
                    Character &huohuo = state.allies[huohuoPos];
                    master.cleanseDebuff();
                    int masterPos = searchCharacter(state.allies, master.name);
                    singleHeal(state, huohuo, masterPos, 0.045, 120);
                };
                ally.effects.push_back(skillEfx);
            }

            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Tail: Spiritual Domination
            slowPrint("フォフォ：こ、来ないで...\n",self.nameColor);
            slowPrint("",{},500);
            slowPrint("シッポ：うろちょろと目障りなんだよ...悪鬼は、俺様だけで充分だ！\n",self.nameColor);
            for (auto & allie : state.allies) {
                Effect tail("Spiritual Domination", "buff", 2,0);
                allie.effects.push_back(tail);
                if (allie.name!=self.name) {
                    double charge = allie.maxEnergy * 0.2;
                    addEnergy(allie,charge);
                } else addEnergy(self,5);
                double increased_atk = allie.baseAtk * 0.2;
                allie.atk += increased_atk;
                tail.values.push_back(increased_atk);
                tail.endRound = [](Effect &self, Character &master, State &state) {
                    self.duration--;
                    if (self.duration == 0) {
                        master.atk -= self.values[0];
                        master.removeEffect((self));
                    }
                };
            }
        };
    }
}

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
        int situationNo, level = 1;
        double speed, hp, atk, def;
        char comma;

        getline(ss, name, ',');
        ss >> situationNo >> comma;
        ss >> speed >> comma;
        ss >> hp >> comma;
        ss >> atk >> comma;
        ss >> def;

        Character temp(name, level, speed, hp, atk, def, 0, 0, 0, 0, 0);
        temp.faction = "enemy";
        insertEnemyAbility(temp);
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
        int taunt, level, maxHp;

        getline(ss, name, ',');
        ss >> level >> comma;
        ss >> speed >> comma;
        ss >> hp >> comma;
        ss >> atk >> comma;
        ss >> def >> comma;
        ss >> critRate >> comma;
        ss >> critDamage >> comma;
        ss >> maxEnergy >> comma;
        ss >> taunt;
        maxHp = hp;

        Character temp(name, level, speed, hp, atk, def, critRate, critDamage, maxEnergy, taunt, maxHp);
        insertCharacterAbility(temp);
        playableCharacters.push_back(temp);
    }
    return playableCharacters;
}

int levelscaling(double statistic, int level){
    int newstat = static_cast<int>(statistic * 7.4 * level - statistic * level - statistic * 7.4 + 80 * statistic) / 79;
    return newstat;
}

void enemyscaling(vector <Character> &enemy, int level){
    for (auto & character : enemy){
        character.level = level;
        character.hp = levelscaling(character.baseHp, level);
        character.atk = levelscaling(character.baseAtk, level);
        character.def = levelscaling(character.baseDef, level);
    }
}