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
	Can set up "task forces" or another better name. Can then assign task forces of task forces to manipulate on the field
	Set up and plan invasions
	Maybe a supply line that you'd have to maintain? That'd be hard tho
*/

#include <SDL.h>
#include <time.h>
#include <stdio.h>

#include "components.h"
#include "scene.h"
#include "systems.h"
#include "gameState.h"
#include "terrain.h"

#include "arraylist.h"

int main(int argc, char** argv) 
{
	game_init();
	Terrain* terrain = terrain_create(16*64);
	Scene* match = Scene_Create(Components_Init);

	Transform testTransform = {
		(struct vector) {
			0.0f, 0.0f
		},
		0.0f,
		(struct vector) {
			0.0f, 0.0f
		},
		(struct vector) {
			0.0f, 0.0f
		},
		(struct vector) {
			0.0f, 0.0f
		},
		0.0f
	};

	SimpleRenderable rend = {
		NULL,
		NULL,
		false
	};
	
	for (int i = 0; i < 1000; i++)
	{
		EntityID test = Scene_NewEntity(match);
		Scene_Assign(match, test, TRANSFORM_ID, &testTransform);
		Scene_Assign(match, test, SIMPLE_RENDERABLE_ID, &rend);
	}


	long previous = clock();
	long lag = 0;
	long current = clock();
	long elapsed = 0;
	long dt = 16;
	int elapsedFrames = 0;

	unsigned int frames = 0;

	Uint64 start = SDL_GetPerformanceCounter();
	while (g->running) 
	{
		current = clock();
		elapsed = current - previous;

		previous = current;
		lag += elapsed;

		elapsedFrames += lag;

		while (lag >= dt) {
			game_pollInput();
			// update entities
			System_Transform(match);
			terrain_update(terrain);
			lag -= dt;
		}
		if (elapsedFrames > 32) {
			game_beginDraw();
			elapsedFrames = 0;
			terrain_render(terrain);
			System_Render(match);
			game_endDraw();
		}
		frames++;
		const Uint64 end = SDL_GetPerformanceCounter();
		Uint64 freq = SDL_GetPerformanceFrequency();
		const double seconds = (end - start) / (float)(freq);
		if (seconds > 5.0)
		{
			// MUST be under 16,000 micro seconds
			printf("%d frames in %f seconds = %f FPS(%f us/frame)\n", frames, seconds, frames/seconds, (seconds * 1000000.0) / frames);
			start = end;
			frames = 0;
		}
	}

	return 0;
}