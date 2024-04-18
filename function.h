#ifndef PROJECT_FUNCTION_H
#define PROJECT_FUNCTION_H
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include "state.h"

using namespace std;

void printHelp(const string& name);
void slowPrint(const string& text, const vector<int>& sgr={0}, int delayMS=10);
int selectTarget(vector<Character>& characters);
vector<Character> getPlayableCharacters();
vector<vector<Character>> getSituation();
string setDescription(int i, const string& requirement);
#endif //PROJECT_FUNCTION_H
