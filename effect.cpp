#include "effect.h"

#include <utility>

using namespace std;

Effect::Effect(string name, string type, int duration, int stack): name(std::move(name)), duration(duration), stack(stack),type(std::move(type)) {}