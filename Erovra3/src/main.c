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

A complex system that slowly comes to an equilibrium with one nation remaining. Goal is to be that nation remaining
*/

#include "./main.h"
#include "./assemblages/components.h"
#include "./gui/gui.h"
#include "./scenes/menu.h"
#include "./scenes/tournament.h"
#include "./util/debug.h"
#include "./util/heap.h"
#include "./util/lexicon.h"
#include "engine/apricot.h"
#include "textures.h"
#include <SDL.h>
#include <stdio.h>
#include <string.h>

//#define CREATE_LEXICON
//#define CREATE_TEXTURES

/*
	Entry point of program. Inits the game, then starts the game loop */
int SDL_main(int argc, char** argv)
{
#ifdef CREATE_LEXICON
    int status;
    Lexicon_Create("res/countryNames.txt", "res/countryNames.lex", &status);
#endif
    Apricot_Init("Erovra 3.10.2", 1280, 720);
#ifdef CREATE_TEXTURES
    Textures_Draw();
#endif
    GUI_Init();
    Components_Init();
    Menu_Init();
    Textures_Init();
    Apricot_Run();
    return 0;
}