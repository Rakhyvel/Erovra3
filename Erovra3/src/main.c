/*
main.c

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



Code guidelines:
	Messy logic is fine as long as it is necessary
	Long functions are fine as long as they are necessary
	Similar functions that can be parameratized into one function should be parameritized into one function
	Utility functions (not systems) that are only called from one place, and are not recursive, should be inlined
	Update comments for a function when you change something in the method
	Add line comments to a piece of code when you spend time trying to understand a piece of code
	Each file should have a header comment giving its purpose
	Each function should have a comment should tell what it takes in, does, and returns
	THE LESS LINES OF CODE THE BETTER!



*/

#include "./main.h"
#include "./entities/components.h"
#include "./gui/gui.h"
#include "./scenes/menu.h"
#include "./scenes/tournament.h"
#include "engine/apricot.h"
#include "textures.h"
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