#include "function.h"
#include <iostream>
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

/* Print text slowly, with a delay between each character
 * text: the text to print
 * delayMS: the delay between each character in milliseconds
 * sgr: a vector of SGR codes to apply to the text. See https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters */
void slowPrint(const string& text, int delayMS, const vector<int>& sgr) {
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
        cout << c;
        this_thread::sleep_for(chrono::milliseconds(delayMS));
    }
    cout << "\033[0m";
}

