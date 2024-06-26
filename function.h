#ifndef PROJECT_FUNCTION_H
#define PROJECT_FUNCTION_H
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include "state.h"
#include "situation.h"

using namespace std;

void printHelp(const string& name);
void printDescription(const string &name);
void printCharacterPortrait(const string& name);
void printCharacterByLine(const string& name,int charaColor, int line);
void monkeyLock(State &state, Character &self, bool F= true);
void slowPrint(const string& text, const vector<int>& sgr={0}, int delayMS=10);
int searchCharacter(const vector<Character>& characters, const string& target);
int selectTargetPrompt(vector<Character>& characters);
vector<Character> getPlayableCharacters();
vector<Situation> getSituations();
int levelscaling(double statistic, int level);
void enemyscaling(vector <Character> &enemy, int level);
void printMainCommandSelected(int num);
void addEnergy(Character &character, double energy);

#endif //PROJECT_FUNCTION_H
