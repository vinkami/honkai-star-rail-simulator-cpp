#ifndef PROJECT_FUNCTION_H
#define PROJECT_FUNCTION_H
#include <string>
#include <chrono>
#include <thread>
using namespace std;

void printHelp(const string& name);
void slowPrint(const string& text, int delayMS=50);

#endif //PROJECT_FUNCTION_H
