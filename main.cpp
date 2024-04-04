#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class State;

class Character{
public:
    string name;
    int speed;
    double resetRemTime;
    double remTime;

    Character(string name, int speed) {
        this->name = name;
        this->speed = speed;
        this->resetRemTime = 10000.0 / speed;
        this->remTime = 15000.0 / speed;  // First round is longer

    };

    void forward(double time) {
        this->remTime = this->remTime - time;
        if (this->remTime < 0.0) this->remTime = 0.0;
    }

    void reset(){
        this->remTime = this->resetRemTime;
    }

    virtual void action() {
        cout << name + " attacks." << endl;
    }
};

class Round: public Character {
    public:
    Round(): Character("Round", 100) {};
    void action() {
        cout << "Round starts." << endl;
    }
};

class State {
public:
    Character **allies;
    Character **enemies;
    Character *round;

    int maxSkillPoint;
    int skillPoint;
    int roundNumber;

    State(Character **allies, Character **enemies): round(new Round()) {
        this->allies = allies;
        this->enemies = enemies;
        this->maxSkillPoint = 5;
        this->skillPoint = 3;
        this->roundNumber = 0;
    };

    void forward(double time) {
        for (int i = 0; i < 4; i++) {
            this->allies[i]->forward(time);
        }
        for (int i = 0; i < 5; i++) {
            this->enemies[i]->forward(time);
        }
        this->round->forward(time);
    }

    Character *nextCharacter() const {
        Character *next = this->allies[0];
        for (int i = 1; i < 4; i++) {
            if (this->allies[i]->remTime < next->remTime) {
                next = this->allies[i];
            }
        }
        for (int i = 0; i < 5; i++) {
            if (this->enemies[i]->remTime < next->remTime) {
                next = this->enemies[i];
            }
        }
        if (this->round->remTime < next->remTime) {
            next = this->round;
        }
        return next;
    };
};


bool gameLoop(State *state) {
    string line;
    getline(cin, line);
    stringstream ss(line);
    string cmd;

    while (ss >> cmd) {
        if (cmd == "exit") {
            return false;
        } else {
            Character *now = state->nextCharacter();
            now->action();
            double remTime = now->remTime;
            state->forward(remTime);
            now->reset();
        }
    }
    return true;
}

int main() {
    Character *allies[4] = {new Character("A1", 133), new Character("A2", 90), new Character("A3", 170), new Character("A4", 136)};
    Character *enemies[5] = {new Character("E1", 100), new Character("E2", 200), new Character("E3", 300), new Character("E4", 400), new Character("E5", 500)};
    State state(allies, enemies);

    while(gameLoop(&state));
}