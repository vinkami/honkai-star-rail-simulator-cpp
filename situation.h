#ifndef PROJECT_SITUATION_H
#define PROJECT_SITUATION_H

#include <string>
#include "character.h"

using namespace std;

class Situation {
public:
    string name, description;
    vector<Character> enemies{};

    Situation(string  name, string  description);
};

#endif //PROJECT_SITUATION_H
