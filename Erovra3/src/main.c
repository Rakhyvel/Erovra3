/*
main.c

This game should be a game that you can play at work. Like a solataire/mine sweeper/
minecraft type game. Shouldn't be something that "immerses" you in it explicitly
- Shouldn't need network connectivity to play core game

starting state: A capital, and a unit that you can use to expand
themes: resource management, production management, strategic planning and logistics
goals:		(1) destroy enemy capital	(2) protect own capital
obstacles:	(1) enemy defenses			(2) enemy offense

Resources:
	Ore is scarce. You need ore to build units, which you need to attack/protect capitals. 
	Population is also scarce. Should be able to train/retrain people for different skills. Used up when unit is purchase

Production management:
	Different units have different trade offs
	Should be anti air, heavy light and medium tanks, different classes of boats, and airplanes
	Factories should be able to build things like bombs, wings, guns; intermediate products for units

Strategy and logistics:
	Can set up "task forces" or another better name (formation?). Can then assign task forces of task forces to manipulate on the field
	Set up and plan invasions
	Maybe a supply line that you'd have to maintain? That'd be hard tho
*/

#include "./main.h"
#include "./entities/components.h"
#include "./gui/gui.h"
#include "./scenes/menu.h"
#include "./scenes/tournament.h"
#include "engine/apricot.h"
#include "textures.h"
#include "./util/heap.h"
#include "./util/debug.h"
#include <SDL.h>
#include <stdio.h>
#include <string.h>

/*
	Inits the game, then starts the game loop */
int SDL_main(int argc, char** argv)
{
    Apricot_Init("Erovra 3.9.4", 1280, 680);
    GUI_Init();
    Components_Init();
    Textures_Init();
#ifdef TOURNAMENT
    Tournament_Init();
#else
    Menu_Init();
#endif
    Apricot_Run();
    return 0;
}