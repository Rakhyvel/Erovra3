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
#include <stdio.h>
#include <time.h>

#include "./components/city.h"
#include "./components/components.h"
#include "./components/infantry.h"
#include "./components/nation.h"
#include "./engine/gameState.h"
#include "./engine/scene.h"
#include "./systems/systems.h"
#include "terrain.h"
#include "textures.h"

#include "./util/arraylist.h"

/*
	Creates a new scene, adds in two nations, capitals for those nations, and infantries for those nation */
Scene* startMatch(Terrain* terrain)
{
    Scene* match = Scene_Create(Components_Init);

    EntityID homeNation = Nation_Create(match, (SDL_Color) { 60, 100, 250 });
    EntityID enemyNation = Nation_Create(match, (SDL_Color) { 250, 80, 80 });

    EntityID homeCapital = City_Create(match, findBestLocation(terrain, (Vector) { terrain->size, terrain->size }), homeNation, true);
    EntityID enemyCapital = City_Create(match, findBestLocation(terrain, (Vector) { 0, 0 }), enemyNation, true);

    EntityID homeInfantry = Infantry_Create(match, GET_COMPONENT_FIELD(match, homeCapital, TRANSFORM_COMPONENT_ID, Transform, pos), homeNation);

    terrain_setOffset(GET_COMPONENT_FIELD(match, homeCapital, TRANSFORM_COMPONENT_ID, Transform, pos));
    // Set enemy nations to each other
    SET_COMPONENT_FIELD(match, homeNation, NATION_COMPONENT_ID, Nation, enemyNation, enemyNation);
    SET_COMPONENT_FIELD(match, enemyNation, NATION_COMPONENT_ID, Nation, enemyNation, homeNation);
    // Set nations capitals
    SET_COMPONENT_FIELD(match, homeNation, NATION_COMPONENT_ID, Nation, capital, homeCapital);
    SET_COMPONENT_FIELD(match, enemyNation, NATION_COMPONENT_ID, Nation, capital, enemyCapital);

	return match;
}

/*
	Inits the game, then starts the game loop */
int main(int argc, char** argv)
{
    Game_Init("Erovra", 1166, 640);
    Textures_Init();
    Terrain* terrain = terrain_create(16 * 64);
    Scene* match = startMatch(terrain);

    long previous = clock();
    long lag = 0;
    long current = clock();
    long elapsed = 0;
    long dt = 16;
    int elapsedFrames = 0;

    unsigned int frames = 0;

    Uint64 start = SDL_GetPerformanceCounter();
    while (g->running) {
        current = clock();
        elapsed = current - previous;

        previous = current;
        lag += elapsed;

        elapsedFrames += elapsed;

        while (lag >= dt) {
            Game_PollInput();
            // update entities
            terrain_update(terrain);
            System_Transform(match);
            lag -= dt;
        }
        if (elapsedFrames > 16) {
            Game_BeginDraw();
            elapsedFrames = 0;
            terrain_render(terrain);
            System_Render(match);
            Game_EndDraw();
        }
        frames++;
        const Uint64 end = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();
        const double seconds = (end - start) / (float)(freq);
        if (seconds > 5.0) {
            // MUST be under 16,000 micro seconds
            printf("%d frames in %f seconds = %f FPS(%f us/frame)\n", frames, seconds, frames / seconds, (seconds * 1000000.0) / frames);
            start = end;
            frames = 0;
        }
    }

    return 0;
}