# TODO list + tiny documentation
This is a list of things that need to be done in the project.


## main.cpp
- [ ] include everything needed from battle.cpp and setup.cpp
### commands
- [ ] `team` - go to the team menu -> calls `teamMenu()`
- [ ] `battle` - go to the battle menu -> calls `battleMenu()`
- [ ] `settings` - go to the settings menu -> calls `settingsMenu()`
- [ ] `start` - starts the game -> calls `battleStart()` and while loop `gameLoop()`
- [x] `exit` - exits the game
- [ ] `help` - shows the help menu


## setup.cpp
asks the user what playable characters they want to use and what enemies they want to fight
### `teamMenu()`
- [ ] provides a list of playable characters with their skills and stats
- [ ] ask the user what characters they want to use
### `battleMenu()`
- [ ] provides a list of situations, with a description of the enemies
- [ ] ask the user what situation they want to fight and return the enemies
### `settingsMenu()`
- [ ] difficulty settings


## battle.cpp
handles the battle system
### `battleStart()`
things to be done when the battle starts
### `battleLoop()`
the main loop of the battle  
procedure:
- shows which character moves and how many skill points they have
- if the character is `Round`, display that a new round has started
- if the character is an enemy, choose a random skill from their skill set to a random target
- if the character is a player, ask the user what skill they want to use and what target
- apply the skill to the target and show the results (damage, healing, buff, debuff, etc.)
- return `true` if the battle continues, `false` if the battle ends
### `battleEnd()`
show the results of the battle, remaining health, used rounds, etc.
ask if they want to play again