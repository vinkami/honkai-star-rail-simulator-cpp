#include "function.h"
#include <iostream>
#include <string>
#include <fstream>

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

void slowPrint(const string& text, int delayMS) {
    for (char c : text) {
        cout << c;
        this_thread::sleep_for(chrono::milliseconds(delayMS));
    }
}