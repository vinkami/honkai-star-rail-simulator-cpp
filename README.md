# Honkai: Star Rail Battle Simulator

## Introduction
This is a simulator for the battle in the game Honkai: Star Rail.    
This game consists of various characters and enemies, which all have different skills and stats.  
This is a turn-based RPG game where the speed stat determines how fast each character moves.

## How to start the game
To start the game, run the following command in the terminal in the project directory:
```bash
make
```
Yes this is literally it.  
In the game you can run `help` and see the available commands to work out more.

## Game rules
**TO BE ADDED**

## Stats
Each character and enemy has the following stats:
- Hp: Health Points
- Atk: Attack
- Def: Defense
- Spd: Speed
- Crit%: Critical Rate
- CritDmg: Critical Damage

## Characters
**TO BE ADDED**

## Enemies
**TO BE ADDED**

# Stuff that are required in the group project brief
## Team members
- [Vincent](https://github.com/vinkami)
- [David](https://github.com/IamnotOscar)
- [Sunny](https://github.com/Sanichi52)
- [John](https://github.com/wanndieearly)
- [Jacky](https://github.com/Icescream46)

## Description of the game and game rules
All above.

## Features aka Code Requirements
- Random game events: Enemies choose a random skill and a random target based on aggro system
- Data structures: `vector<Character> allies` and `vector<Character> enemies` to store the characters and enemies
- Dynamic memory management: Many pass-by-reference functions and the vectors you saw above
- File I/O: `loadCharacters()` to load the characters from a file (You can modify the values to manually change the difficulty)
- Program codes in multiple files: `main.cpp`, `setup.cpp`, `battle.cpp`, `character.cpp`, etc.

## Non-standard libraries
**There might be `nlohmann/json` if we decide to use JSON files for the character stats.**  
**TO BE ADDED**

## Compilation and execution instructions
All above.

## Link to demonstration brief video
**TO BE ADDED**