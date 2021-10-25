/*	apricot.h
* 
*	Provides implementation of the scene stack, game loop, and user input 
*	polling.
* 
*	# SCENE STACK
*	The Apricot engine works by using a stack of scenes. The scene at the top
*	of the scene stack has its entities updated and rendered.
* 
*	# USER INPUT
*	User input is polled each tick, and is relevant for that tick. There are many
*	global variables that relate to user input. These should be read only and not
*	written to.
* 
*	@author	Joseph Shimel
*	@date	3/28/21
*/

#pragma once

#include "../util/arraylist.h"
#include "../util/vector.h"
#include "scene.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdbool.h>

#define APRICOT_ARROWKEY (char)1

/* ---------- Game loop ---------- */
/* Whether to run game ASAP or attempt 60 ticks per second */
bool Apricot_ASAP;
/* The number of milleseconds per tick */
double Apricot_DeltaT;
/* Number of ticks that have passed since the application was started */
const int Apricot_Ticks;

/* ---------- Keyboard info ---------- */
/* Character of key that was not held previous tick, or was pressed over a second ago, and is held this tick
   If shift is held, will be character of shifted key for US keyboards.
   If arrow key is pressed, will be (char)1.
*/
const char Apricot_CharDown;
/* Maps scancode to whether or not the scancode is held down this tick */
const bool Apricot_Keys[255];

/* ---------- Mouse info ---------- */
/* If the left mouse button is pressed and has moved at all since being pressed */
const bool Apricot_MouseDrag;
/* If the left mouse button is down this tick */
const bool Apricot_MouseLeftDown;
/* If the left mouse button was down last tick, is up this tick */
const bool Apricot_MouseLeftUp;
/* If the mouse has been moved since the last tick */
const bool Apricot_MouseMoved;
/* If the right mouse button is down this tick */
const bool Apricot_MouseRightDown;
/* If the right mouse button was down last tick, is up this tick */
const bool Apricot_MouseRightUp;
/* Position of the mouse when the left mouse button was first clicked */
const Vector Apricot_MouseInit;
/* Position of the mouse, relative to the upper-left corner of the application window */
const Vector Apricot_MousePos;
/* Mouse wheel offset */
const Vector Apricot_MouseWheel;

/* ---------- SDL structs ---------- */
/* Hardware accelarated graphics context for the application */
const SDL_Renderer* const Apricot_Renderer;
/* Represents the window of the application */
const SDL_Window* const Apricot_Window;

/* ---------- Window info ---------- */
/*	Height of the window in pixels */
const int Apricot_Height;
/*	Width of the window in pixels */
const int Apricot_Width;

/**
 * @brief Initializes the SDL, sound, and scene stack
 * 
 * @param windowTitle Title for the window
 * @param width Width of the window in pixels
 * @param height Height of the window in pixels
*/
void Apricot_Init(char* windowTitle, int width, int height);

/**
 * @brief Pushes a scene onto the scene stack. Stales the scene stack, though it is perfectly fine to keep working on the stale scene.
 * @param scene New scene to push onto the stack
*/
void Apricot_PushScene(Scene* scene);
/**
 * @brief Pops off a number of scenes from the scene stack. Stales the current scene! Immediately break out of scene update after calling this function.
 * @param numScenes How many scenes to pop off the scene stack
*/
void Apricot_PopScene(int numScenes);
/**
* @brief Runs the game loop forever until exit() is called.
*/
void Apricot_Run();