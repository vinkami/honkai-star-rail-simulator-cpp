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


void printCommandSelected(vector<string> list, int num) {
    for (int i=0;i<9;i++){
        if (num == i) slowPrint("["+list[i]+"]", {31}, 0);
        else cout << " " << list[i] << " ";
        if (i !=8) cout << " | ";
        else cout << endl;
    }
}

void printMainCommandSelected(int num) {
    vector<string> list = {"help", "start", "team", "list", "battle", "level", "settings", "restart", "quickstart"};
    printCommandSelected(list, num);
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

void monkeyLock(State &state, Character &self, bool F){
    if (self.faction!="ally") return;
    Effect monkey = Effect("Monkey Eye On You","other",-1,0);
    bool flag= true;
    for (const auto &enemy : state.enemies){
        if (enemy.name=="Malefic Ape")
            flag=false;
    } if (flag && F) return;
    if (state.movement=='e' || not F){
        for (auto &ally : state.allies){
            ally.removeEffect(monkey);
        }
        self.effects.push_back(monkey);
        slowPrint("Malefic Ape marks on "+self.name+"!\n",{31});
    }
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

void checkDot(State &state,Character &self){
    std::vector<Character>& team = (self.faction == "ally") ? state.enemies : state.allies;
    for (auto &effect:self.effects){
        if (effect.isDot)
            dot(team[effect.values[0]],self,effect.skillMultiplier,effect.name);
    }
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
                slowPrint("Invalid target. Defaulting to 1\n");
                return 0;
            } else {
                return target;
            }
        } catch (invalid_argument& e) {
            slowPrint("Invalid target. Defaulting to 1\n");
            return 0;
        } catch (out_of_range& e) {
            slowPrint("Invalid target. Defaulting to 1\n");
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
        enemy.nameColor={37};
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            slowPrint("Antibaryon: 消滅\n",{31});
            singleAttack(state, self, target, 2.5);
            state.timelineProceed = true;
        };
    } else if (enemy.name== "Trampler"){
        enemy.nameColor={37};
        enemy.basicAtk = [](Character &self, State &state){
            Effect lockOn("lockOn", "other", -1, 1);  // idk what happen
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
                    slowPrint("Trampler: 螺旋の弓矢\nTrampler locks on to "+state.allies[target].name+"!\n"+"Cast End of Bow to this target in the next action.\n",{31});
                    state.allies[target].effects.push_back(lockOn);
                } else if (hit(37.5)){
                    slowPrint("Trampler: 侵略の鉄蹄\n",{31});
                    singleAttack(state,self,target,4.0);
                } else if (hit(60)){
                    slowPrint("Trampler: 戦争の蹂躙\n",{31});
                    blastAttack(state,self,target,2.5,2.5);
                }else {
                    slowPrint("Trampler: 虚実の投影\n",{31});
                    singleAttack(state,self,target,3.0);
                }
            }
            state.timelineProceed = true;
        };
    } else if (enemy.name== "Reaver"){
        enemy.nameColor={37};
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

    }else if (enemy.name== "Golden Hound"){
        enemy.nameColor={37};
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            slowPrint("Golden Hound: 角抵\n", {31});
            singleAttack(state, self, target, 2.5);
            state.timelineProceed = true;
        };
        enemy.onHit = [](Character&self, State &state, Character &attacker){
            if (self.hp<=0){
                slowPrint("遠吠え",{31});
                //TODO: change next-move character to the character next to golden hound
            }
        };
    } else if (enemy.name== "Malefic Ape"){
        Effect monkeTarget = Effect("元気", "other", -1, 0);
        Effect monkeRage = Effect("奮威","buff",-1,0);
        enemy.effects.push_back(monkeTarget);
        enemy.effects.push_back(monkeRage);
        enemy.nameColor={35};
        enemy.basicAtk = [](Character &self, State &state) {
            int target = aiTarget(state.allies);
            for (int i=0;i<state.allies.size();i++){
                int index = state.allies[i].getEffectLoc("Monkey Eye On You");
                if (index!=-1){
                    target=i;
                }
            }
            Effect &genki = self.getEffectOrCrash("元気");
            Effect &wakuwaku = self.getEffectOrCrash("奮威");

            if (genki.stack==0){
                slowPrint("Malefic Ape: 怒哮\n",self.nameColor);
                slowPrint("Malefic Ape gains 3 stacks of Gusto.\n",self.nameColor);
                monkeyLock(state,state.allies[target],false);
                slowPrint("Any target that uses their Skill will become the new Monitored target.\n",self.nameColor);
                genki.stack+=3;
            } else {
                double increased_atk = self.baseAtk*0.2;
                self.atk += increased_atk*wakuwaku.stack;
                wakuwaku.values.push_back(increased_atk);
                wakuwaku.endRound = [](Effect &self, Character &master, State &state) {
                    self.duration--;
                    if (self.duration == 0) {
                        master.atk -= self.values[0];
                        master.removeEffect((self));
                    }
                };
                if (wakuwaku.stack<3){
                    slowPrint("Malefic Ape: 奮威\n",self.nameColor);
                    slowPrint("Malefic Ape boosted its damage\n",self.nameColor);
                    wakuwaku.stack+=1;
                }

                genki.stack-=1;
                slowPrint("Malefic Ape: 獣撃\n",self.nameColor);
                singleAttack(state,self, target,4.75);
                if (genki.stack==0)
                    slowPrint("Malefic Ape is exhausted\n",self.nameColor);
            }
            state.timelineProceed = true;
        };
    } else if (enemy.name== "Wooden Lupus"){
        enemy.speed=120;
        Effect howl = Effect("howling","other",-1,1);
        enemy.effects.push_back(howl);
        enemy.nameColor={36};
        enemy.basicAtk = [](Character &self, State &state) {
            Effect &summon=self.getEffectOrCrash("howling");
            Effect bilibili = Effect("Shocked","debuff",3,1);
            bilibili.isDot=true;
            bilibili.skillMultiplier=1.5;
            bilibili.values.push_back(searchCharacter(state.enemies,self.name));
            bilibili.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            int target= aiTarget(state.allies);
            if (hit(100) || summon.stack==0) {
                slowPrint("Wooden Lupus: 狼餐\n", self.nameColor);
                singleAttack(state, self, target, 2);
                if (hit(80) && state.allies[target].hp>0) {
                    int con=state.allies[target].getEffectLoc("Shocked");
                    slowPrint(state.allies[target].name+" is Shocked for 3 turns.\n",{31});
                    if (con==-1)
                        state.allies[target].effects.push_back(bilibili);
                    else
                        state.allies[target].effects[con].duration=3;
                }
            } else {
                slowPrint("Wooden Lupus: 嘯合\n",self.nameColor);
                slowPrint("Wooden Lupus summoned Shadow Jackhyena!\n",self.nameColor);
                summon.stack-=1;
                Character wolf= Character("Shadow Jackhyena",self.level,120,self.hp,155,700,0,0,0,0,self.maxHp);
                wolf.maxHp=self.maxHp; wolf.faction="enemy";
                insertEnemyAbility(wolf);
                state.enemies.push_back(wolf);
            }
            state.timelineProceed = true;
        };
    }  else if (enemy.name== "Shadow Jackhyena"){
        enemy.nameColor={36};
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            slowPrint("Shadow Jackhyena: 狼餐\n", self.nameColor);
            singleAttack(state, self, target, 2);
            //TODO:dot atk
            state.timelineProceed = true;
        };
    }  else if (enemy.name== "Mara-Struck Soldier") {
        enemy.nameColor={36};
        Effect regen = Effect("Mara-Struck","other",0,1);
        regen.endRound = [](Effect &self, Character &master, State &state) {
            if(master.hp >= 0 ) {
                master.hp += master.baseHp*0.5;
                cout<<self.name<<" regen "<<endl;
                self.stack--;
            }
            if(self.stack==0) {
                master.removeEffect(self);
            }
        };
        enemy.basicAtk = [](Character &self, State &state) {
            int target = aiTarget((state.allies));
            singleAttack(state, self, target,2);
            Effect DoT = Effect("Wind Shear","debuff",3,2);
            int self_pos = searchCharacter(state.enemies,self.name);
            DoT.values.push_back(self_pos);
            DoT.endRound = [](Effect &self, Character &master, State &state) {
                int master_pos = searchCharacter(state.allies,master.name);
                self.duration--;
                singleAttack(state,state.enemies[self.values[0]],master_pos,0.5);
                if (self.duration==0) {
                    master.removeEffect(self);
                }
            };
            //addEnergy(self,15);
            if(hit(85)) {
                state.allies[target].effects.push_back(DoT);
            }
            state.timelineProceed = true;
        };
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
        Effect syzygy("Syzygy", "other", -1, 0);
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
                state.timelineProceed = true;
                if (syzygy.stack >= 2) {
                    transmigration.stack = 1;
                    self.critRate += 50;
                    self.remTime = 0;
                    state.timelineProceed = false;  // 100% speed up
                    slowPrint("鏡流：乗月返真。 (Spectral Transmigration mode activated)\n", self.nameColor);
                }
            } else {  // Moon On Glacial River
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
                state.timelineProceed = true;
            }
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
            state.timelineProceed=true;
        };
        character.ult = [](Character &self, State &state) {  // Amidst the Rejoicing Clouds
            int target = selectTarget(state.allies);
            Character &allies = state.allies[target];
            slowPrint("停云:万事吉とならん、一心同帰\n",  self.nameColor);
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
    //Huohuo

    //Kafka
    else if (character.name == "Kafka") {
        character.nameColor ={35};
        character.basicAtk = [](Character &self, State &state) {  // Lucent Moonglow
            int target = selectTarget(state.enemies);
            slowPrint("一瞬よ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        //Todo: follow up attack
        character.skill = [](Character &self, State &state) {
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            //skill point check
            int target = selectTarget(state.enemies);
            singleAttack(state, self, target, 2.0);
            blastAttack(state, self, target, 0.75, 0.75);
            //slowPrint((" ")); seem there are no lyris when kafka use skill
            self.energy += 30;
            //if target contain elecshock
            // std::vector<Character> &team = (attacker.faction == "ally") ? state.enemies : state.allies;
            if (target > 0) {
                Character &left_target = state.enemies[target - 1];
                if(left_target.getEffectLoc("電觸") != -1) {
                    singleAttack(state, self, target-1,0.825);
                }
            }
            if (target + 1 < state.enemies.size()) {
                Character &right_target = state.enemies[target + 1];
                if(right_target.getEffectLoc("電觸") != -1) {
                    singleAttack(state, self, target+1,0.825);
                }
            }
            state.timelineProceed = true;
            //check contain effect
        };
        character.ult = [](Character &self, State &state) {
            //int character_pos = searchCharacter(state.allies, self.name);
            aoeAttack(state, self, 0.8);
            slowPrint("お別れの時間よ…ポン！\n",self.nameColor);
            aoeAttack(state, self, 3.47);
            //ult attack end
            //effect
            Effect elecshock = Effect("電觸", "debuff" ,2);
            double rounddmg = self.atk * 3.47;
            double oringaldmg = self.atk * 1.08;
            elecshock.values.push_back(rounddmg);//elecshock round damage
            elecshock.values.push_back(oringaldmg);//oringnal damage
            // elecshock value[0] = rounddmg
            // elecshock value[1] = oringnaldmg
            elecshock.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                master.hp -= self.values[0];
                slowPrint("素晴らしい時は…やがて過ぎ去る。\n",{35});
                if(self.duration==0) {
                    master.removeEffect(self);
                }
            };
            for(auto &enemy: state.enemies) {
                if(hit(75)) {
                    enemy.effects.push_back(elecshock);
                    //singleAttack(state, self, );
                    //enemy.hp-= elecshock.values[1];
                }
            }
        };
    }
    //kafka

    //FuXuan
    else if (character.name == "Fuxuan") {
        //todo: add telent team damage resist 18%
        character.nameColor ={35};
        //talent Fuxuan self healing
        Effect bulf = Effect("陰陽転換、生々流転。", "other", 0, 1);
        bulf.endRound = [](Effect &self, Character &master, State &state) {
            if(master.hp<master.baseHp*0.5 && master.hp>0) {
                master.hp+= (master.baseHp-master.hp)*0.9;
                slowPrint("陰陽転換、生々流転。",{35});
                self.stack--;
            }
        };
        character.effects.push_back(bulf);
        character.basicAtk = [](Character &self, State &state) {  // Lucent Moonglow
            int target = selectTarget(state.enemies);
            slowPrint("極数，知来！\n成象，効法！\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };

        character.skill =[](Character &self, State &state) {
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            // Doing Effect for shar damage
            slowPrint("うよいったい\nしょうかしょうい\n",self.nameColor);
            addEnergy(self, 30);
            Effect share_dmg = Effect("穷观阵", "buff", 3);
            int Fuxuan_pos = searchCharacter(state.allies, "Fuxuan");
            double addiction_hp = self.baseHp * 0.6;
            double addiction_critrate = 12;
            share_dmg.values.push_back(Fuxuan_pos);
            share_dmg.values.push_back(addiction_hp);
            share_dmg.values.push_back(addiction_critrate);
            //giving all ally Fuxuan's skill buff
            for (auto &ally : state.allies) {
                if(ally.name!= "Fuxuan") {
                    int ally_pos= searchCharacter(state.allies,ally.name);
                    if(ally.getEffectLoc("穷观阵")>=0) {
                        ally.critRate+=addiction_critrate;
                        ally.baseHp+=addiction_hp;
                        ally.effects.push_back(share_dmg);
                    }
                    if(ally.effects[ally.getEffectLoc("穷观阵")].duration<3) {
                        ally.effects[ally.getEffectLoc("穷观阵")].duration=3;
                    }
                }
            }
            // Fuxuan_pos at share_dmg.valus[0] , addi hp [1], addi critrate [2]
            //todo: Finish Fuxuan share damage skill for each charater
            share_dmg.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                int master_pos = searchCharacter(state.allies, master.name);
                double hp_difference = self.values[0] - master.hp;
                singleHeal(state,state.allies[self.values[0]],master_pos,0, hp_difference * 0.65);
                self.values[0] = master.hp;
                if(self.duration==0) {
                    master.baseHp -= self.values[1];
                    master.critRate -= self.values[2];
                    //removing baseHp
                    if(master.hp>master.baseHp) {
                        master.hp = master.baseHp;
                    }
                    master.removeEffect((self));
                }
            };
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {
            //todo: the aoeAttack of fuxuan should base on baseHp value 100%
            aoeAttack(state, self,1);
            slowPrint("換斗移星、人事を、成す！",self.nameColor);
            if(self.effects[self.getEffectLoc("陰陽転換、生々流転。")].stack<2) {
                slowPrint("世の万物に法理あり…",self.nameColor);
                self.effects[self.getEffectLoc("陰陽転換、生々流転。")].stack+=1;
            }
            int self_pos = searchCharacter(state.allies , "Fuxuan");
            for (auto &ally : state.allies) {
                int ally_pos = searchCharacter(state.allies,ally.name);
                if(state.allies[ally_pos].name != "Fuxuan"){
                    singleHeal(state, state.allies[self_pos],ally_pos,0.05,133);
                }
            }
        };
    }
    //FuXuan

    //Natasha
    else if (character.name == "Natasha") {
        character.nameColor ={97};
        character.basicAtk = [](Character &self, State &state) {  // Lucent Moonglow

            int target = selectTarget(state.enemies);
            slowPrint("重症ね。\n",  self.nameColor);
            slowPrint("生命の重みを味わいなさい。\n",  self.nameColor);
            slowPrint("眠るといいわ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };

        character.skill =[](Character &self, State &state) {
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            slowPrint("薬を飲みましょう。\n",  self.nameColor);
            slowPrint("痛くないでしょ。\n",self.nameColor);
            addEnergy(self, 30);
            int target = selectTarget(state.allies);
            Character &ally = state.allies[target];
            int self_pos = searchCharacter(state.allies, self.name);
            double healing_hp = self.baseHp*0.105 + 280;
            double healing_hp_round = self.baseHp*0.072 + 192;
            //add decision below 30%hp
            if (ally.hp <= ally.baseHp*0.3) {
                singleHeal(state,self,target,0.105*1.5,280*1.5);
                //ally.hp+= healing_hp * 1.5;
            }else {
                singleHeal(state,self,target,0.105,280);
                //ally.hp += healing_hp;
            }
            //Healing of skill
            //Healing Effect last 2 round
            Effect healing = Effect("Love, Healing and Decison", "buff", 2);
            healing.values.push_back(healing_hp);
            healing.values.push_back(healing_hp_round);
            healing.values.push_back(self_pos);
            //values[0] == healing_hp
            //values[1] == healing_hp_round
            //value[2] == natasha pos
            healing.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                int master_pos = searchCharacter(state.allies,master.name);
                //determination
                if (master.hp <= master.baseHp*0.3) {
                    singleHeal(state,state.allies[self.values[2]],master_pos,0.072*1.5,192*1.5);
                    //master.hp += self.values[1] * 1.5;
                }else {
                    singleHeal(state,state.allies[self.values[2]],master_pos,0.072,192);
                    //master.hp += self.values[1];
                }
                if(self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            //Effect declaring end
            //negative effect removing
            ally.effects.push_back(healing);
            for(Effect &effect_in_charater : ally.effects) {
                if(effect_in_charater.type=="debuff") {
                    ally.removeEffect(effect_in_charater);
                }
            }
            //end turn
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {
            for(auto &ally : state.allies) {
                int ally_pos = searchCharacter(state.allies,ally.name);
                //double healing_hp_ult = self.hp * 0.138  + 368;
                //ally.hp += healing_hp_ult;
                if (ally.hp <= ally.baseHp*0.3) {
                    singleHeal(state,self,ally_pos,0.138*1.5,368*1.5);
                    //ally.hp+= healing_hp_ult * 1.5;
                }else {
                    singleHeal(state,self,ally_pos,0.138,368);
                    //ally.hp += healing_hp_ult;
            }
            }
        };
    }
    //Natasha

    //Asta
    else if (character.name == "Asta") {
        //todo: add asta buff team attack buff
        character.nameColor ={31};
        Effect astrometry = Effect("Astrometry","buff",-1,0);
        astrometry.endRound = [](Effect &self, Character &master, State &state){

        };
        character.effects.push_back(astrometry);
        character.basicAtk = [](Character &self, State &state) {
            Effect &astro=self.getEffectOrCrash("Astrometry");
            int target = selectTarget(state.enemies);
            if (astro.stack<5) astro.stack+=1;
            Character &enemy = state.enemies[target];
            slowPrint("アスター: 避けてね。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            //basic attack effect
            Effect buring = Effect("burn","debuff",3,0);

            int Asta_pos =searchCharacter(state.allies,"Asta");
            buring.values.push_back(Asta_pos);
            buring.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                int master_pos = searchCharacter(state.allies, master.name);
                self.skillMultiplier=0.5;self.isDot= true;
                if (self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            //adding effect on target by chance
            if (hit(80)) {
                if (enemy.getEffectLoc("burn")==-1) enemy.effects.push_back(buring);
                else enemy.getEffectOrCrash("burn").duration=3;
            }
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self , State &state) {
            Effect &astro=self.getEffectOrCrash("Astrometry");
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            if (hit(50)) slowPrint("アスター: ラッキースターは誰かな～\n",self.nameColor);
            else slowPrint("アスター: 星の祝福を貴方に～\n",  self.nameColor);
            int target = selectTarget((state.enemies)), add;
            Character &enemy = state.enemies[target];
            bounceAttack(state, self, target, 0.5, 4,add);
            if (add+astro.stack>5) astro.stack=5;
            else astro.stack+=add;
            addEnergy(self, 30);
            state.timelineProceed = true;

        };
        character.ult = [](Character &self , State &state) {
            addEnergy(self,5);
            slowPrint("アスター: 星の秘密を求めるカギよ、開拓者たちに真なる祝福を！\n",  self.nameColor);
            Effect ult_speed_up = Effect("星空の願い", "buff", 2, 0);
            ult_speed_up.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration==0) {
                    master.speed-=50;
                    master.removeEffect(self);
                }
            };
            for (Character &ally: state.allies) {
                if (ally.getEffectLoc("星空の願い")==-1){
                    ally.effects.push_back(ult_speed_up);
                    ally.speed+= 50;
                } else ally.getEffectOrCrash("星空の願い").duration=2;
            }
        };
        //Asta
    }
        //Danheng
    else if (character.name == "Danheng") {
        character.nameColor ={92};
        character.basicAtk = [](Character &self, State &state) {
            int target = selectTarget(state.enemies);
            slowPrint("丹恒: 今だ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            if (hit(50))
                slowPrint("丹恒: 争いに...意味はない。\n",self.nameColor);
            else slowPrint("丹恒: どけ、騒ぎを起こしたくはない\n",self.nameColor);
            int target = selectTarget(state.enemies);
            Character &enemy = state.enemies[target];
            singleAttack(state, self, target, 2.6);
            addEnergy(self, 30);
            Effect speed_lo = Effect("疾雨", "debuff", 2, 0);
            speed_lo.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.speed += master.baseSpeed * 0.12;
                    master.removeEffect(self);
                }
            };
            if (hit(70)) {
                if (enemy.getEffectLoc("疾雨") == -1) {
                    enemy.effects.push_back(speed_lo);
                    enemy.speed -= enemy.baseSpeed * 0.12;
                } else enemy.getEffectOrCrash("疾雨").duration = 2;
                state.timelineProceed = true;
            }
        };

        character.ult = [](Character &self, State &state) {
            slowPrint("丹恒: 生死虚実、一念の間なり。\n",  self.nameColor);
            int target = selectTarget(state.enemies);
            Character &enemy = state.enemies[target];
            slowPrint("丹恒: 洞天幻化、長夢一覚...破！\n",  self.nameColor);
            if(enemy.effects[enemy.getEffectLoc("疾雨")].duration>0) {
                singleAttack(state, self, target, 4*1.2);
            } else {
                singleAttack(state, self, target, 4);
            }
        };
    }
    //Danheng
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

        Character temp(name, level, speed, hp, atk, def, 0, 0, 0, 0,0);
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
        maxHp = static_cast<int>(hp);

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