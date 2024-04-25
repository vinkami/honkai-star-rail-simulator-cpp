#include "effect.h"

using namespace std;

Effect::Effect(string name, int duration, int stack,bool type): name(std::move(name)), duration(duration), stack(stack),type(type) {}