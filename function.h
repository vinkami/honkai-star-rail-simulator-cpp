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
void slowPrint(const string& text, const vector<int>& sgr={0}, int delayMS=10);
int searchCharacter(const vector<Character>& characters, const string& target);
int selectTarget(vector<Character>& characters);
vector<Character> getPlayableCharacters();
vector<Situation> getSituations();
int levelscaling(double statistic, int level);

#endif //PROJECT_FUNCTION_H
