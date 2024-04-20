#include "effect.h"

using namespace std;

Effect::Effect(string name, int duration, int stack): name(std::move(name)), duration(duration), stack(stack) {}