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
#### Exit
Quits the game.
#### Help
Shows the help menu, provides details on all the commands.
#### Start
Starts the game, only works when you have a team snd battle selected.
#### Team
Let you select the characters you would put in battle. You should select 4 for your team.
#### List
Shows the list of playable characters available, and detailed profiles of them.
#### Battle
Let you select the battle scenario you would like to fight in.
#### Level
Shows the leveling sleextion menu, you can increase the levels to increase the game's difficulty.
#### Settings
Shows the settings menu.
#### Restart
Restarts the game
#### Quickstart
Start the game with a predetermined team and battle.
### During battle
#### Help
Shows the help menu, shows all the commands.
#### Check
Check the effects and stats of the allies or enemies.
#### Escape
Escape from the fight.
#### Reset
Restart with all same setting.
#### Exit
Exit the battle menu and return to the main menu.
#### Q
Do a normal attack.
#### E
Release the skill of the current character
#### 1,2,3,4
Release the ultimate of the character at the corresponding position, only when the ultimate energy bar is full.
For most normal attacks, skills and ultimates, you will be prompted to select a target.

## Stats
Each character and enemy has the following stats:
- Hp: Health Points
- Atk: Attack
- Def: Defense
- Spd: Speed
- Crit%: Critical Rate
- CritDmg: Critical Damage

## Characters
### Jingliu
### Huohuo
### Kafka
### Clara
### Tingyun
### Fuxuan
### Natasha
### Asta
### Danheng
### Sparkle

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

## Credits
- Ascii art: some of the art was modified from ascii.co.uk
- Table: Table function from github.com/friedmud/variadic_table
## Link to demonstration video
**TO BE ADDED**