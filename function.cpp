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
        cout << "Please be aware that the description is from the original Honkai: Star Rail, which may not be totally accurate in this game." << endl;
        cout << description.rdbuf();
    } else{
        cout << "Character not found." << endl;
    }
    description.close();
}
//void printAllCharactersPortrait(vector<Character> allies, vector<Character> enemies){
//    vector<char> currentCharacter;
//    currentCharacter.insert(currentCharacter.end(),allies.begin().name,allies.end().name);
//
//
//}
void printCharacterPortrait(const string &name) {
    ifstream portraitFile("art/" + name + ".txt");
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

void printCharacterByLine(const string &name,int charaColor, int line) {
    ifstream portraitFile("art/" + name + ".txt");
    if (portraitFile.is_open()) {
        string l;
        int i=0;
        while (getline(portraitFile, l)) {
            if (i==line) slowPrint(l,{charaColor},0);
            i++;
        }
        cout << "   ";
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
    Effect monkey = Effect("Monitor","other",self,-1,0);
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

int selectTargetPrompt(vector<Character>& characters) {
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
            Effect lockOn("lockOn", "other", self, -1, 1);  // idk what happen
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
                } else {
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
        enemy.nameColor = {34};
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            if (hit(50)) {
                slowPrint("AutomatonSpider: 突進！\n",self.nameColor);
                singleAttack(state, self, target, 2.0);
            } else {
                slowPrint("AutomatonSpider: 時限モジュール!!!\nThe next action uses \"自爆モジュール\"\n");
                //idk is there any lyris so i put in megumin lyris from konosuba
                Effect explosion = Effect("Self-destruct", "other", self, 1, 0);
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

    } else if (enemy.name== "Golden Hound"){
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
        Effect monkeTarget = Effect("Gusto", "other", enemy, -1, 0);
        Effect monkeRage = Effect("Enhance","buff", enemy, -1,0);
        enemy.effects.push_back(monkeTarget);
        enemy.effects.push_back(monkeRage);
        enemy.nameColor={35};
        enemy.basicAtk = [](Character &self, State &state) {
            int target = aiTarget(state.allies);
            for (int i=0;i<state.allies.size();i++){
                int index = state.allies[i].getEffectLoc("Monitor");
                if (index!=-1){
                    target=i;
                }
            }
            Effect &genki = self.getEffectOrCrash("Gusto");
            Effect &wakuwaku = self.getEffectOrCrash("Enhance");

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
                    slowPrint("Malefic Ape: Enhance\n",self.nameColor);
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
        Effect howl = Effect("howling","other",enemy,-1,1);
        enemy.effects.push_back(howl);
        enemy.nameColor={36};
        enemy.basicAtk = [](Character &self, State &state) {
            Effect &summon=self.getEffectOrCrash("howling");
            Effect bilibili = Effect("Shocked","debuff",self,3,1);
            bilibili.values.push_back(self.atk * 1.2);  // atk to calculate damage
            bilibili.startRound = [](Effect &self, Character &master, State &state) {
                dot(self, master);
                self.duration--;
                if (self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            int target= aiTarget(state.allies);
            if (summon.stack==0) {
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
                Character wolf= Character("Shadow Jackhyena",self.level,120,self.hp,155,700,0,0,0,0, self.maxHp);
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
        Effect regen = Effect("Mara-Struck","other",enemy,0,1);
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
            Effect DoT = Effect("Wind Shear","debuff",self,3,2);
            DoT.values.push_back(self.atk);
            DoT.startRound = [](Effect &self, Character &master, State &state) {
                dot(self, master);
                self.duration--;
                if (self.duration==0) {
                    master.removeEffect(self);
                }
            };
            if(hit(85)) {
                state.allies[target].effects.push_back(DoT);
            }
            state.timelineProceed = true;
        };
    } else if(enemy.name == "Sweet Gorilla"){
        enemy.nameColor={93};
        enemy.basicAtk = [](Character &self, State &state) {

            Effect lowerDef= Effect("DEF reduction","debuff",self,2,0);
            lowerDef.endRound = [](Effect &self, Character &master, State &state) {
                self.stack-=1;
                if(self.stack==0) {
                    master.def+=self.values[0];
                    master.removeEffect(self);
                }
            };

            Effect freeDrinkCharge = Effect("Charge","other",self,-1,0);
            int target= aiTarget(state.allies);
            bool drinkCharge=false;
            if (self.getEffectLoc("Charge")!=-1) drinkCharge=true;
            if (drinkCharge){
                Effect &charge = self.getEffectOrCrash("Charge");
                self.dmgReduction-=charge.values[0];
                self.removeEffect(charge);
                slowPrint("スウィート．ゴリラ: フリードリンク（飲み放題）\n",self.nameColor);
                for (int i=0;i<4;i++){
                    target= aiTarget(state.allies);
                    Character &defender=state.allies[target];
                    singleAttack(state,self,target,2.0);
                    if (hit(90)){
                        slowPrint(defender.name+"'s defense is decreased.\n",{31});
                        if (defender.getEffectLoc("DEF reduction")==-1) {
                            double decreased_def = defender.baseDef*0.2;
                            lowerDef.values.push_back(decreased_def);
                            defender.effects.push_back(lowerDef);
                            defender.def-=decreased_def;
                        } else {
                            Effect freeDrink = defender.getEffectOrCrash("DEF reduction");
                            freeDrink.duration=2;
                        }
                    }
                }
            }else if (hit(50)) {     //normal atk
                Character &defender=state.allies[target];
                slowPrint("スウィート．ゴリラ: フリードリンク（数量限定）\n",self.nameColor);
                singleAttack(state, self, target, 3.8);
                if (hit(90)){
                    slowPrint(defender.name+"'s defense is decreased.\n",{31});
                    if (defender.getEffectLoc("DEF reduction")==-1) {
                        int decreased_def = static_cast<int>(defender.baseDef*0.2);
                        lowerDef.values.push_back(decreased_def);
                        defender.effects.push_back(lowerDef);
                        defender.def-=decreased_def;
                    } else {
                        Effect freeDrink = defender.getEffectOrCrash("DEF reduction");
                        freeDrink.duration=2;
                    }
                }
            } else {        //charge
                slowPrint("スウィート．ゴリラ: スラーダ噴水\n",self.nameColor);
                slowPrint("Sweet Gorilla enters the Charge state and gains Toughness Protection.\nWhen taking action the next time, uses フリードリンク（飲み放題）.\n",self.nameColor);
                double increased_dmg_reduction = 0.2;
                freeDrinkCharge.values.push_back(increased_dmg_reduction);
                self.effects.push_back(freeDrinkCharge);
                self.dmgReduction+=increased_dmg_reduction;
            }
            state.timelineProceed = true;
        };
    } else if (enemy.name == "Bubble Hound"){
        enemy.nameColor={93};
        Effect dogTalent = Effect("Hearty Revelry","other",enemy,-1,0);
        dogTalent.startRound = [](Effect &self, Character &master, State &state){
            for (auto &effect:master.effects){if (effect.type=="debuff") self.stack+=1;}
            if (self.stack>4) self.stack=4;
            double increased_atk=master.baseAtk*self.stack*0.4;
            master.atk+=increased_atk;
            self.values.push_back(increased_atk);
        };
        dogTalent.endRound = [](Effect &self, Character &master, State &state){
            master.atk-=self.values[0];
            self.values={};
            self.stack=0;
        };
        enemy.effects.push_back(dogTalent);
        enemy.basicAtk = [](Character &self, State &state) {
            int target= aiTarget(state.allies);
            slowPrint("ソーダドッグ: 飲みの誘い\n",self.nameColor);
            singleAttack(state, self, target, 2.5);
            state.timelineProceed = true;
        };
    } else if (enemy.name=="Birdskull"){
        enemy.nameColor={33};
        enemy.basicAtk = [](Character &self, State &state) {
            Effect bleeding = Effect("Bleeding","debuff",self,2,1);
            bleeding.startRound = [](Effect &self, Character &master, State &state) {
                dot(self, master,2);
                self.duration--;
                if (self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            int target= aiTarget(state.allies);
            Character &defender = state.allies[target];
            slowPrint("踊る仮面の鳥: 舞踏への誘い\n");
            singleAttack(state, self, target, 2.5);
            if (hit(50)){
                if (defender.getEffectLoc("Bleeding")!=-1) defender.effects.push_back(bleeding);
                else defender.getEffectOrCrash("Bleeding").duration=2;
                slowPrint(defender.name+" is bleeding.\n");
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
        Effect enhancedCounter = Effect("Enhanced Counter", "other", character, -1, 0);
        character.effects.push_back(enhancedCounter);
        character.dmgReduction = 0.1;
        character.basicAtk = [](Character &self, State &state) {  // I Want to Help
            int target = selectTargetPrompt(state.enemies);
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
            Effect ultEfx("Promise, Not Command", "buff", self, 2, 0);
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
            Effect counter("Mark of Counter", "other",self, -1, 1);
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
        Effect syzygy("Syzygy", "other", character, -1, 0);
        Effect transmigration("Spectral Transmigration", "other", character, -1, 0);
        character.effects.push_back(syzygy);
        character.effects.push_back(transmigration);

        character.basicAtk = [](Character &self, State &state) {  // Lucent Moonglow
            Effect &transmigration = self.getEffectOrCrash("Spectral Transmigration");
            if (transmigration.stack == 1) {
                slowPrint("Basic Attack is disabled during Spectral Transmigration mode.\n",{91});
                return;
            }
            int target = selectTargetPrompt(state.enemies);
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
                int target = selectTargetPrompt(state.enemies);
                slowPrint("鏡流：飛光よ、差せ！\n", self.nameColor);
                syzygy.stack += 1;
                slowPrint("Syzygy stack: " + to_string(syzygy.stack) + "\n",self.nameColor);
                singleAttack(state, self, target, 2.0);
                addEnergy(self,20);
                state.timelineProceed = true;
                if (syzygy.stack >= 2 && transmigration.stack == 0) {
                    transmigration.stack = 1;
                    self.critRate += 50;
                    self.remTime = 0;
                    state.timelineProceed = false;  // 100% speed up
                    slowPrint("鏡流：乗月返真。 (Spectral Transmigration mode activated)\n", self.nameColor);
                }
            } else {  // Moon On Glacial River
                // does not consume skill point
                int target = selectTargetPrompt(state.enemies);
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
            int target = selectTargetPrompt(state.enemies);
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
            if (syzygy.stack >= 2 && transmigration.stack == 0) {
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
            int target = selectTargetPrompt(state.enemies);
            slowPrint("落ち着いて。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self,20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Soothing Melody
            Effect benediction("Benediction", "buff", self, 3, 0);  // benediction for allies
            Effect speed_up ("Speed up", "buff", self, 1, 0);  // self speed up
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            // benediction part
            slowPrint("停云:万事順調～\n停云：諸悪退散～\n",  self.nameColor);
            int target = selectTargetPrompt(state.allies);// selecting target
            Character &allies = state.allies[target];
            double increased_atk = allies.baseAtk * 0.5;
            if (increased_atk > self.atk*0.25) {
                increased_atk = self.atk*0.25;
            }
            benediction.values.push_back(increased_atk);
            allies.effects.push_back(benediction);
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
            self.effects.push_back(speed_up);
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
            int target = selectTargetPrompt(state.allies);
            Character &allies = state.allies[target];
            slowPrint("停云:万事吉とならん、一心同帰\n",  self.nameColor);
            addEnergy(allies,50);
            Effect ultEfx("Amidst the Rejoicing Clouds", "buff", self, 2, 0);
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
            int target = selectTargetPrompt(state.enemies);
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
            int targetPos = selectTargetPrompt(state.allies);
            Character &target = state.allies[targetPos];
            if (hit(50)) slowPrint("フォフォ：邪を払い...魅を縛らん...\n", self.nameColor);
            else slowPrint("フォフォ：霊符よ...守りたまえ...\n", self.nameColor);
            addEnergy(self,30);
            target.cleanseDebuff();
            blastHealing(state, self, targetPos, 0.21, 0.168, 560, 448);
            // divine provision - added to allies instead for simplicity
            for (auto &ally: state.allies) {
                Effect skillEfx("Divine Provision", "buff", self, 2, 1);
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
            for (auto & ally : state.allies) {
                Effect tail("Spiritual Domination", "buff", self, 2,0);
                ally.effects.push_back(tail);
                if (ally.name != self.name) {
                    double charge = ally.maxEnergy * 0.2;
                    addEnergy(ally, charge);
                } else addEnergy(self,5);
                double increased_atk = ally.baseAtk * 0.2;
                ally.atk += increased_atk;
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
        character.basicAtk = [](Character &self, State &state) {  // Midnight Tumult
            int target = selectTargetPrompt(state.enemies);
            slowPrint("カフカ: 一瞬よ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Caressing Moonlight
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            //skill point check
            int target = selectTargetPrompt(state.enemies);
            blastAttack(state, self, target, 1.6, 0.6);
            //slowPrint((" ")); seem there are no lyris when kafka use skill
            addEnergy(self, 30);
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Twilight Trill
            //int character_pos = searchCharacter(state.allies, self.name);
            slowPrint("カフカ: 素晴らしい時は…やがて過ぎ去る。\n", self.nameColor);
            slowPrint("カフカ: お別れの時間よ…ポン！\n",self.nameColor);
            aoeAttack(state, self, 0.8);
            //ult attack end
            //effect
            Effect elecshock = Effect("Electric Shock", "debuff" , self, 2);
            elecshock.values.push_back(self.atk);//elecshock round damage
            elecshock.startRound = [](Effect &self, Character &master, State &state) {
                dot(self, master);
                self.duration--;
                if(self.duration==0) {
                    master.removeEffect(self);
                }
            };
            for(auto &enemy: state.enemies) {
                if(hit(75)) {
                    if (enemy.getEffectLoc("Electric Shock")==-1)
                        enemy.effects.push_back(elecshock);
                    else enemy.getEffectOrCrash("Electric Shock").duration=2;
                    slowPrint(enemy.name+ " is Shocked",self.nameColor,0);
                    //singleAttack(state, self, );
                    //enemy.hp-= elecshock.values[1];
                }
            }
        };
    }
    //kafka

    //FuXuan
    else if (character.name == "Fuxuan") {
        character.nameColor ={35};
        //talent Fuxuan self healing
        Effect hpRestoration = Effect("HP Restoration", "other", character, 0, 1);
        character.effects.push_back(hpRestoration);
        character.startRound = [](Character &self, State &state) {
            for (auto &efx: self.effects) efx.startRound(efx, self, state);
            Effect dmgReduction = Effect("Misfortune Avoidance", "buff", self, -1, 1);
            dmgReduction.startRound = [](Effect &self, Character &master, State &state) {
                if (self.applier->hp <= 0) {
                    master.removeEffect(self);
                    master.dmgReduction -= .18;
                }
            };
            for (auto &ally: state.allies) {
                if (ally.name == "Fuxuan") continue;
                if (ally.getEffectLoc("Misfortune Avoidance") == -1) {
                    ally.effects.push_back(dmgReduction);
                    ally.dmgReduction += .18;
                }
            }
        };

        character.basicAtk = [](Character &self, State &state) {  // Novaburst
            int target = selectTargetPrompt(state.enemies);
            slowPrint("符玄：極数，知来！\n符玄：成象，効法！\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };

        character.skill =[](Character &self, State &state) {  // Known by Stars, Shown by Hearts
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            slowPrint("符玄：相与一体\n符玄：上下象易\n",self.nameColor);

            // Fu Xuan gets the Matrix of Prescience, other allies get Knowledge
            Effect matrix("Matrix of Prescience", "buff", self, 3, 1);
            matrix.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    for (auto &ally: state.allies) {
                        if (&ally == &master) continue;
                        if (ally.getEffectLoc("Knowledge") == -1) continue;  // should not happen; just to be safe
                        Effect &efx = ally.getEffectOrCrash("Knowledge");
                        ally.baseHp -= efx.values[0];
                        ally.critRate -= 12;
                        ally.removeEffect(efx);
                    }
                    master.removeEffect(self);
                }
            };
            self.effects.push_back(matrix);

            for (auto &ally: state.allies) {
                if (&ally == &self) continue;
                if (ally.getEffectLoc("Knowledge") == -1) {
                    Effect knowledge = Effect("Knowledge", "buff", self, 1, 1);
                    knowledge.values.push_back(self.baseHp * 0.06);  // [0] base HP increase
                    ally.baseHp += self.baseHp * 0.06;
                    ally.critRate += 12;
                    ally.effects.push_back(knowledge);
                }
            }
            addEnergy(self, 30);
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Woes of Many Morphed to One
            aoeAttack(state, self,1);
            slowPrint("符玄：換斗移星、人事を、成す！\n",self.nameColor);
            if (self.effects[self.getEffectLoc("HP Restoration")].stack < 2) {
                slowPrint("符玄：世の万物に法理あり…\n",self.nameColor);
                self.effects[self.getEffectLoc("HP Restoration")].stack += 1;
            }
            int self_pos = searchCharacter(state.allies , "Fuxuan");
            for (auto &ally : state.allies) {
                int ally_pos = searchCharacter(state.allies,ally.name);
                if(state.allies[ally_pos].name != "Fuxuan"){
                    singleHeal(state, state.allies[self_pos],ally_pos,0.05,133);
                }
            }
        };
        character.onHit = [](Character &self, State &state, Character &attacker) {
            Effect &hpRestore = self.getEffectOrCrash("HP Restoration");
            if (self.hp < self.baseHp * 0.5 && self.hp > 0 && hpRestore.stack > 0) {
                self.hp+= (self.baseHp-self.hp)*0.9;
                slowPrint("符玄：陰陽転換、生々流転。\n",{35});
                hpRestore.stack--;
            }
        };
    }
    //FuXuan

    //Natasha
    else if (character.name == "Natasha") {
        character.nameColor = {97};
        character.basicAtk = [](Character &self, State &state) {  // Behind the Kindness
            int target = selectTargetPrompt(state.enemies);
            slowPrint("重症ね。\n",  self.nameColor);
            slowPrint("生命の重みを味わいなさい。\n",  self.nameColor);
            slowPrint("眠るといいわ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            state.incSkillPoint();
            state.timelineProceed = true;
        };

        character.skill =[](Character &self, State &state) {  // Love, Heal, and Choose
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            slowPrint("薬を飲みましょう。\n",  self.nameColor);
            slowPrint("痛くないでしょ。\n",self.nameColor);
            addEnergy(self, 30);
            int target = selectTargetPrompt(state.allies);
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
            Effect healing = Effect("Love, Healing and Decison", "buff", self, 2);
            healing.startRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                int master_pos = searchCharacter(state.allies,master.name);
                //determination
                if (master.hp <= master.baseHp*0.3) {
                    singleHeal(state,*self.applier,master_pos,0.072*1.5,192*1.5);
                } else {
                    singleHeal(state,*self.applier,master_pos,0.072,192);
                }
                if(self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            //Effect declaring end
            //negative effect removing
            ally.effects.push_back(healing);
            ally.cleanseDebuff();
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // Gift of Rebirth
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
        Effect astrometry = Effect("Astrometry","buff",character,-1,0);
        astrometry.endRound = [](Effect &self, Character &master, State &state){

        };
        character.effects.push_back(astrometry);
        character.basicAtk = [](Character &self, State &state) {  // Spectrum Beam
            Effect &astro=self.getEffectOrCrash("Astrometry");
            int target = selectTargetPrompt(state.enemies);
            if (astro.stack<5) astro.stack+=1;
            Character &enemy = state.enemies[target];
            slowPrint("アスター: 避けてね。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 20);
            //basic attack effect
            Effect burning = Effect("burn", "debuff", self, 3, 0);

            burning.startRound = [](Effect &self, Character &master, State &state) {
                dot(self, master);
                self.duration--;
                if (self.duration == 0) {
                    master.removeEffect(self);
                }
            };
            //adding effect on target by chance
            if (hit(80)) {
                if (enemy.getEffectLoc("burn")==-1) enemy.effects.push_back(burning);
                else enemy.getEffectOrCrash("burn").duration=3;
            }
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self , State &state) {  // Meteor Storm
            Effect &astro=self.getEffectOrCrash("Astrometry");
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            if (hit(50)) slowPrint("アスター: ラッキースターは誰かな～\n",self.nameColor);
            else slowPrint("アスター: 星の祝福を貴方に～\n",  self.nameColor);
            int target = selectTargetPrompt((state.enemies)), add;
            bounceAttack(state, self, target, 0.5, 4,add);
            if (add+astro.stack>5) astro.stack=5;
            else astro.stack+=add;
            addEnergy(self, 30);
            state.timelineProceed = true;
        };
        character.ult = [](Character &self , State &state) {  // Astral Blessing
            addEnergy(self,5);
            slowPrint("アスター: 星の秘密を求めるカギよ、開拓者たちに真なる祝福を！\n",  self.nameColor);
            Effect ult_speed_up("Astral Blessing", "buff", self, 2, 0);
            ult_speed_up.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration==0) {
                    master.speed-=50;
                    master.removeEffect(self);
                }
            };
            for (Character &ally: state.allies) {
                if (ally.getEffectLoc("Astral Blessing")==-1){
                    ally.effects.push_back(ult_speed_up);
                    ally.speed+= 50;
                } else ally.getEffectOrCrash("Astral Blessing").duration=2;
            }
        };
        //Asta
    }
        //Danheng
    else if (character.name == "Danheng") {
        character.nameColor ={92};
        character.basicAtk = [](Character &self, State &state) {  // Cloudlancer Art: North Wind
            int target = selectTargetPrompt(state.enemies);
            slowPrint("丹恒: 今だ。\n",  self.nameColor);
            singleAttack(state, self, target, 1.0);
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Cloudlancer Art: Torrent
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", {91});
                return;
            }
            if (hit(50))
                slowPrint("丹恒: 争いに...意味はない。\n",self.nameColor);
            else slowPrint("丹恒: どけ、騒ぎを起こしたくはない\n",self.nameColor);
            int target = selectTargetPrompt(state.enemies);
            Character &enemy = state.enemies[target];
            singleAttack(state, self, target, 2.6);
            addEnergy(self, 30);
            Effect speed_lo = Effect("Slowness", "debuff", self, 2, 0);
            speed_lo.endRound = [](Effect &self, Character &master, State &state) {
                self.duration--;
                if (self.duration == 0) {
                    master.speed += master.baseSpeed * 0.12;
                    master.removeEffect(self);
                }
            };
            if (hit(70)) {
                if (enemy.getEffectLoc("Slowness") == -1) {
                    enemy.effects.push_back(speed_lo);
                    enemy.speed -= enemy.baseSpeed * 0.12;
                } else enemy.getEffectOrCrash("Slowness").duration = 2;
                state.timelineProceed = true;
            }
        };

        character.ult = [](Character &self, State &state) {  // Ethereal Dream
            slowPrint("丹恒: 生死虚実、一念の間なり。\n",  self.nameColor);
            int target = selectTargetPrompt(state.enemies);
            Character &enemy = state.enemies[target];
            slowPrint("丹恒: 洞天幻化、長夢一覚...破！\n",  self.nameColor);
            if (enemy.getEffectLoc("Slowness") == -1) {
                if (enemy.getEffectOrCrash("Slowness").duration>0) {
                    singleAttack(state, self, target, 4*1.2);
                } else {
                    singleAttack(state, self, target, 4);
                }
            }
        };
    } else if (character.name == "Sparkle") {
        character.nameColor = {38, 5, 196};
        character.startBattle = [](Character &self, State &state) {
            state.maxSkillPoint = 7;
            for (auto &ally : state.allies) {
                ally.dmgBonus += .18;  // Talent dmg bonus but simplified
            }
        };
        character.basicAtk = [](Character &self, State &state) {  // Monodrama
            int target = selectTargetPrompt(state.enemies);
            slowPrint("花火：悪役は花火に任せてぇ～\n", self.nameColor);
            singleAttack(state, self, target, 1.0);
            addEnergy(self, 30);  // Major trace
            state.incSkillPoint();
            state.timelineProceed = true;
        };
        character.skill = [](Character &self, State &state) {  // Dreamdiver
            if (!state.decSkillPoint()) {
                slowPrint("No skill points left.\n", self.nameColor);
                return;
            }
            int targetPos = selectTargetPrompt(state.allies);
            if (hit(50)) slowPrint("花火：動揺しちゃだーめ。\n", self.nameColor);
            else slowPrint("花火：頑張ってね？\n", self.nameColor);
            Character &target = state.allies[targetPos];
            if (target.getEffectLoc("Dreamdiver") == -1) {
                Effect dreamdiver("Dreamdiver", "buff", self, 2, 0);
                dreamdiver.values.push_back(self.baseCritDamage * .24 + .45);  // [0] crit damage increase
                target.critDamage += dreamdiver.values[0];
                dreamdiver.startRound = [](Effect &self, Character &master, State &state) {
                    self.duration--;
                    if (self.duration == 0) {
                        master.critDamage -= self.values[0];
                        master.removeEffect(self);
                    }
                };
                target.effects.push_back(dreamdiver);
            } else target.getEffectOrCrash("Dreamdiver").duration = 2;

            if (&target != &self) {
                target.remTime /= 2;
            }
            addEnergy(self, 30);
            state.timelineProceed = true;
        };
        character.ult = [](Character &self, State &state) {  // The Hero with a Thousand Faces
            slowPrint("花火：かくれんぼしよっか。\n", self.nameColor);
            slowPrint("花火：愚者千面、浮世を遊び尽くさん…答え、見つけてごらん？\n", self.nameColor);
            state.incSkillPoint(4);
            for (auto &ally: state.allies) {
                Effect cipher("Cipher", "buff", self, 2, 1);
                ally.dmgBonus += .10;
                cipher.endRound = [](Effect &self, Character &master, State &state) {
                    self.duration--;
                    if (self.duration == 0) {
                        master.dmgBonus -= .10;
                        master.removeEffect(self);
                    }
                };
                ally.effects.push_back(cipher);
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