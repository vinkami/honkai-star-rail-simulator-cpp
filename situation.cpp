#include "situation.h"

#include <utility>

Situation::Situation(string name, string description): name(std::move(name)), description(std::move(description)) {

}
