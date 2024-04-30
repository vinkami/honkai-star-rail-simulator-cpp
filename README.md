# Honkai: Star Rail Battle Simulator

## Introduction
This is a simulator for the battle in the game Honkai: Star Rail.    
This game consists of various characters and enemies, which all have different skills and stats.  
This is a turn-based RPG game where the speed stat determines how fast each character moves.

## How to start the game
To start the game, of course you first need to clone the game files into a directory in your computer
Then, run the following commands in the terminal in the project directory:
```bash
bash ./run.sh
```
Yes, that's all you need to do.  
In the game you can run `help` and see the available commands to work out more.

## How to play
### Outside battle
You can select from the main menu the actions you would like to do, such as selecting your team or opponents.
#exit
Quits the same.
#team
Sects the characters you would put in battle. You should select 4 for your team.
#battle
Selects the battle scenario you would like to fight in.
### During battle
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
There are multiple situations in the game, and each situation has different enemies to fight against.  
When you enter the battle menu, you will be able to see the situations' description as well as the enemies in that situation.  

# Other stuff that are required by the group project brief
## Team members
- [Vincent](https://github.com/vinkami)
- [David](https://github.com/IamnotOscar)
- [Sunny](https://github.com/Sanichi52)
- [John](https://github.com/wanndieearly)
- [Jacky](https://github.com/Icescream46)

## Features aka Code Requirements
- Random game events: Enemies choose a random skill and a random target based on taunt system; allies critical hits
- Data structures: `vector<Character> allies` and `vector<Character> enemies` to store the characters and enemies
- Dynamic memory management: Many pass-by-reference functions and the vectors you saw above
- File I/O: `getPlayableCharacters()` to load the characters from a file (You can actually modify the values in `characters.csv` to manually change the difficulty, but a difficulty setting is in game so it's not recommended)
- Program codes in multiple files: `main.cpp`, `setup.cpp`, `battle.cpp`, `character.cpp`, etc.

## Non-standard libraries
### VariadicTable
- File: `VariadicTable.h`
- Source: https://github.com/friedmud/variadic_table
- Usage: To print the table of characters and enemies in a nice format

## Link to demonstration video
**TO BE ADDED**