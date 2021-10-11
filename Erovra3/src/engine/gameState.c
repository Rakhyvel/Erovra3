/*	game.c
*	
*	@author	Joseph Shimel
*	@date	3/28/21
*/

#include "./gameState.h"
#include "../util/debug.h"
#include "./scene.h"
#include <SDL_mixer.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static bool sceneStale = false; // Whether the scene stack has been modified during update
bool asap = false; // Whether to run game ASAP or aim for 60 ticks per second

/*	Takes in a window title, width, and height. Initializes the global game struct
	variable g by creaating an SDL window and renderer. */
void Game_Init(char* windowName, int width, int height)
{
    atexit(&Game_Exit);
    g = (struct game*)calloc(1, sizeof(struct game));
    if (!g) {
        exit(1);
    }

    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error: Initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Create Window
    g->width = width;
    g->height = height;
    g->window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, g->width, g->height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (g->window == NULL) {
        printf("Error: Creating SDL window: %s\n", SDL_GetError());
        exit(1);
    }

    if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl") == SDL_FALSE) {
        printf("Warning: opengl not set as driver\n");
    }
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE) {
        printf("Didnt work");
    }

    // Create Renderer
    g->rend = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (g->rend == NULL) {
        printf("Error: Creating SDL renderer: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetRenderDrawBlendMode(g->rend, SDL_BLENDMODE_BLEND);

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS, 2, 512);
    Mix_AllocateChannels(4);

    g->running = 1;
    g->up = g->down = g->left = g->right = g->ctrl = g->shift = g->mouseWheelY = g->mouseLeftDown = g->mouseRightDown = g->mouseLeftUp = g->mouseRightUp = g->mouseDrag = g->mouseDragged = g->ticks = 0;
    g->sceneStack = Arraylist_Create(3, sizeof(Scene*));
    printf("Started.");
}

void Game_Exit(void)
{
    Mix_CloseAudio();
    SDL_Quit();
}

/*
	Adds a scene to the top of the scene stack */
void Game_PushScene(Scene* scene)
{
    Arraylist_Add(&g->sceneStack, &scene);
    sceneStale = true;
}

/*
	Removes the scene at the top of the scene stack */
void Game_PopScene(int numScenes)
{
    for (int i = 0; i < numScenes; i++) {
        Scene* oldScene = *(Scene**)Arraylist_Pop(g->sceneStack);
        Scene_Destroy(oldScene);
        sceneStale = true;
    }
}

/*
	Starts the game loop. 
	
	Runs the update method of the scene at the top of the scene stack 60 times / second
	Runs the render method of the scene at the top of the scene stack when possible */
void Game_Run()
{
    long previous = clock();
    double lag = 0;
    long current = clock();
    long elapsed = 0;
    g->dt = 16.0;
    int elapsedFrames = 0;

    unsigned int frames = 0;

    Uint64 start = SDL_GetPerformanceCounter();
    while (g->running) {
        current = clock();
        elapsed = current - previous;

        previous = current;
        lag += elapsed;

        elapsedFrames += elapsed;
        if (g->sceneStack->size < 1) {
            PANIC("No scene added to game");
        }
        Scene* scene = *(Scene**)Arraylist_Get(g->sceneStack, g->sceneStack->size - 1);
        // Reset scene stack flag
        sceneStale = false;

        while (lag >= g->dt || asap) {
            Game_PollInput();
            scene->update(scene);
            if (sceneStale) { // Do not purge entities if scene is stale
                break;
            }

            Scene_Purge(scene);
            lag -= g->dt;
            g->ticks++;

            // Only do one iteration each if asap
            if (asap) {
                break;
            }
        }

        if (!sceneStale && !asap) { // Do not render if scene is stale, nor if going asap
            elapsedFrames = 0;
            SDL_SetRenderDrawColor(g->rend, 21, 21, 21, 255);
            SDL_RenderClear(g->rend);
            scene->render(scene);
            SDL_RenderPresent(g->rend);
            frames++;
        }

        const Uint64 end = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();
        const double seconds = (end - start) / (float)(freq);
        if (seconds > 5.0) {
            // MUST be under 16,000 micro seconds
            printf("%d frames in %f seconds = %f FPS(%f us/frame), %f engine seconds\n", frames, seconds, frames / seconds, (seconds * 1000000.0) / frames, g->ticks / 60.0f);
            start = end;
            frames = 0;
        }
    }
}

static char toshifted(SDL_KeyCode c)
{
    if (c < 0) {
        return -1;
    }
    if (c >= 'a' && c <= 'z') {
        return c + 'A' - 'a';
    }
    switch (c) {
    case '`':
        return '~';
    case '1':
        return '!';
    case '2':
        return '@';
    case '3':
        return '#';
    case '4':
        return '$';
    case '5':
        return '%';
    case '6':
        return '^';
    case '7':
        return '&';
    case '8':
        return '*';
    case '9':
        return '(';
    case '0':
        return ')';
    case '-':
        return '+';
    case '=':
        return '_';
    case '[':
        return '{';
    case ']':
        return '}';
    case '\\':
        return '|';
    case ';':
        return ':';
    case '\'':
        return '"';
    case ',':
        return '<';
    case '.':
        return '>';
    case '/':
        return '?';
    }
    return (char)128;
}

/*
	Polls and handles input from SDL event queue, updates game's state struct */
void Game_PollInput()
{
    // PRE INPUT
    g->mouseLeftUp = false;
    g->mouseRightUp = false;
    g->mouseMoved = false;
    g->lt = false;
    g->gt = false;
    g->keyDown = '\0';

    // DIRECT INPUT
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            g->running = 0;
            break;
        case SDL_WINDOWEVENT:
            SDL_GetWindowSize(g->window, &g->width, &g->height);
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                g->up = 1;
                break;
            case SDL_SCANCODE_DOWN:
                g->down = 1;
                break;
            case SDL_SCANCODE_LEFT:
                g->left = 1;
                break;
            case SDL_SCANCODE_RIGHT:
                g->right = 1;
                break;
            case SDL_SCANCODE_LCTRL:
                g->ctrl = 1;
                break;
            case SDL_SCANCODE_LSHIFT:
                g->shift = 1;
                break;
            case SDL_SCANCODE_COMMA:
                g->lt = 1;
                break;
            case SDL_SCANCODE_PERIOD:
                g->gt = 1;
                break;
            }
            if (!g->keys[event.key.keysym.scancode] || g->keys[event.key.keysym.scancode] < g->ticks + 60) {
                if (g->up || g->down || g->left || g->right) {
                    g->keyDown = 1;
                } else if (g->shift && event.key.keysym.scancode != SDL_SCANCODE_LSHIFT) {
                    g->keyDown = toshifted(SDL_GetKeyFromScancode(event.key.keysym.scancode));
                } else {
                    g->keyDown = SDL_GetKeyFromScancode(event.key.keysym.scancode);
                }
            }
            g->keys[event.key.keysym.scancode] = g->ticks;
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                g->up = 0;
                break;
            case SDL_SCANCODE_DOWN:
                g->down = 0;
                break;
            case SDL_SCANCODE_LEFT:
                g->left = 0;
                break;
            case SDL_SCANCODE_RIGHT:
                g->right = 0;
                break;
            case SDL_SCANCODE_LCTRL:
                g->ctrl = 0;
                break;
            case SDL_SCANCODE_LSHIFT:
                g->shift = 0;
                break;
            case SDL_SCANCODE_COMMA:
                g->lt = 0;
                break;
            case SDL_SCANCODE_PERIOD:
                g->gt = 0;
                break;
            }
            g->keys[event.key.keysym.scancode] = 0;
            break;
        case SDL_MOUSEMOTION:
            g->mouseMoved = true;
            SDL_GetMouseState(&(g->mouseX), &(g->mouseY));
            if (event.button.button == SDL_BUTTON_LEFT) {
                g->mouseDrag = true;
            }
            break;
        case SDL_MOUSEWHEEL:
            g->mouseWheelX += event.wheel.x;
            g->mouseWheelY += event.wheel.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                g->mouseLeftDown = true;
                SDL_GetMouseState(&(g->mouseInitX), &(g->mouseInitY));
                break;
            case SDL_BUTTON_RIGHT:
                g->mouseRightDown = true;
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                if (g->mouseLeftDown) {
                    g->mouseLeftUp = true;
                }
                g->mouseLeftDown = false;
                g->mouseDrag = false;
                break;
            case SDL_BUTTON_RIGHT:
                if (g->mouseRightDown) {
                    g->mouseRightUp = true;
                }
                g->mouseRightDown = false;
                break;
            }
            break;
        }
    }

    // POST INPUT
    g->mouseDragged = abs(g->mouseInitX - g->mouseX) > 16 || abs(g->mouseInitY - g->mouseY) > 16;
}