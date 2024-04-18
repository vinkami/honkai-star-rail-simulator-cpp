#ifndef PROJECT_FUNCTION_H
#define PROJECT_FUNCTION_H
#include <string>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

void printHelp(const string& name);
void slowPrint(const string& text, int delayMS=50, const vector<int>& sgr={0});
bool hit(double chance);

#endif //PROJECT_FUNCTION_H
