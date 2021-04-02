/*
main.c
*/

#include <SDL.h>
#include <stdio.h>
#ifdef __cplusplus 
#undef __cplusplus 
#endif

#include "game.h"
#include "terrain.h"

int main(int argc, char** argv) {
	game_init();
	struct terrain* terrain = terrain_create(5*64, g);

	long previous = clock();
	long lag = 0;
	long current = clock();
	long elapsed = 0;
	long dt = 16;
	int elapsedFrames = 0;

	unsigned int frames = 0;
	Uint64 start = SDL_GetPerformanceCounter();

	while (1) {
		current = clock();
		elapsed = current - previous;

		previous = current;
		lag += elapsed;

		elapsedFrames += lag;

		while (lag >= dt) {
			game_pollInput();
			// update entities
			terrain_update(terrain);
			lag -= dt;
		}
		if (elapsedFrames > 32) {
			game_beginDraw();
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			elapsedFrames = 0;
			terrain_render(terrain);
			game_endDraw();
		}
		frames++;
		const Uint64 end = SDL_GetPerformanceCounter();
		Uint64 freq = SDL_GetPerformanceFrequency();
		const double seconds = (end - start) / (float)(freq);
		if (seconds > 2.0)
		{
			printf("%d frames in %f seconds = %f FPS(%f ms/frame)\n", frames, seconds, frames/seconds, (seconds * 1000.0) / frames);
			start = end;
			frames = 0;
		}
	}

	return 0;
}