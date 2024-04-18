#include "effect.h"

using namespace std;

Effect::Effect(string name, int duration): name(std::move(name)), duration(duration) {}