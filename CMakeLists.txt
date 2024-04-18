cmake_minimum_required(VERSION 3.27)
project(Project)

set(CMAKE_CXX_STANDARD 11)

add_executable(Project main.cpp battle.cpp character.cpp state.cpp setup.cpp function.cpp effect.cpp logic.cpp)