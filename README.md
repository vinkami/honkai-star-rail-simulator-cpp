# Honkai: Star Rail Battle Simulator

## Notes for COMP2113/ENGG1340 Newcomers
Yes, this is one of the group projects done for this course.  
This is group 39 of the 2023/2024 semester 2, getting the grade 14/15.  

The project deadline was 30 Apr 2024. Just in case if the repository is modified after that point, you can do:
```bash
git clone https://github.com/vinkami/honkai-star-rail-simulator-cpp
git reset --hard 3128854
```
to get the version exactly at the point where it is submitted.  
At this version there are still quite a few bugs like repeated DoT checking and random crashing, but the results did show that it's not too shabby :D  
Feel free to look around to get inspirations, just don't forget to mention this project in the citations and don't plagiarize.  
--[vinkami](https://github.com/vinkami)

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
Shows the leveling selection menu, you can change the levels of your characters.
#### Settings
Shows the settings menu, you can change the levels and difficulty of the enemies.
#### Restart
Starts the game with the settings of the last game.
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
-Characters and enemies: inspired by the game Honkai Star Rail by Mihoyo hsr.hoyoverse.com/
- Ascii art: some of the art was modified from ascii.co.uk
- Table: Table function from friedmud github.com/friedmud/variadic_table
## Link to demonstration video
- https://connecthkuhk-my.sharepoint.com/:f:/g/personal/u3622713_connect_hku_hk/EgE_0wQxkhBDnc4aEoDhYlYBpzCvhVmTT25nYZPvd4Ksfw?e=Mr83u2
