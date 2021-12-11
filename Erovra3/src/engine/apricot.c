/*	apricot.c
* 
*	@author	Joseph Shimel
*	@date	3/28/21
*/

#include "./apricot.h"
#include "../util/debug.h"
#include "./scene.h"
#include <SDL_mixer.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static bool sceneStale = false; // Whether the scene stack has been modified during update
bool ignoreMissedTicks = false;
static Arraylist* sceneStack;
static Mix_Chunk* samples[2];

/*	--- Const bypass pointers --- 
	Allows game engine to modify const global variables */

static SDL_Window** const _window = &Apricot_Window;
static SDL_Renderer** const _renderer = &Apricot_Renderer;

static Vector* const _mousePos = &Apricot_MousePos;
static Vector* const _mouseInit = &Apricot_MouseInit;
static Vector* const _mouseWheel = &Apricot_MouseWheel;

static int* const _width = &Apricot_Width;
static int* const _height = &Apricot_Height;
static int* const _ticks = &Apricot_Ticks;

static char* const _charDown = &Apricot_CharDown;

static bool* const _keys = Apricot_Keys;
static bool* const _mouseDrag = &Apricot_MouseDrag;
static bool* const _mouseLeftDown = &Apricot_MouseLeftDown;
static bool* const _mouseLeftUp = &Apricot_MouseLeftUp;
static bool* const _mouseMoved = &Apricot_MouseMoved;
static bool* const _mouseRightDown = &Apricot_MouseRightDown;
static bool* const _mouseRightUp = &Apricot_MouseRightUp;

static void apricotExit();
static char toshifted(SDL_KeyCode c);
static void pollInput();

void Apricot_Init(char* windowTitle, int width, int height)
{
    atexit(&apricotExit);

    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error: Initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Create Window
    *_width = width;
    *_height = height;
    *_window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, Apricot_Width, Apricot_Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (Apricot_Window == NULL) {
        printf("Error: Creating SDL window: %s\n", SDL_GetError());
        exit(2);
    }

    if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl") == SDL_FALSE) {
        printf("Warning: opengl not set as driver\n");
    }
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_FALSE) {
        printf("Didnt work");
    }

    // Create Renderer
    *_renderer = SDL_CreateRenderer(Apricot_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (Apricot_Renderer == NULL) {
        printf("Error: Creating SDL renderer: %s\n", SDL_GetError());
        exit(3);
    }
    SDL_SetRenderDrawBlendMode(Apricot_Renderer, SDL_BLENDMODE_BLEND);

    // Init sound
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS, 2, 512);
    Mix_AllocateChannels(4);

    // Init scene stack
    sceneStack = Arraylist_Create(3, sizeof(Scene*));

    printf("Started.");
}

void Apricot_PushScene(Scene* scene)
{
    Arraylist_Add(&sceneStack, &scene);
    sceneStale = true;
}

void Apricot_PopScene(int numScenes)
{
    for (int i = 0; i < numScenes; i++) {
        Scene* oldScene = *(Scene**)Arraylist_Pop(sceneStack);
        Scene_Destroy(oldScene);
        sceneStale = true;
    }
}

void Apricot_Run()
{
    long previous = clock();
    double lag = 0;
    long current = clock();
    long elapsed = 0;
    Apricot_DeltaT = 16.0;
    int elapsedFrames = 0;

    unsigned int frames = 0;

    Uint64 start = SDL_GetPerformanceCounter();
    while (true) {
        current = (clock() * 1000) / CLOCKS_PER_SEC;
        elapsed = current - previous;

        previous = current;
        lag += elapsed;

        elapsedFrames += elapsed;
        if (sceneStack->size < 1) {
            PANIC("No scene added to game");
        }
        Scene* scene = *(Scene**)Arraylist_Get(sceneStack, sceneStack->size - 1);
        // Reset scene stack flag
        sceneStale = false;

        while (lag >= Apricot_DeltaT || Apricot_ASAP) {
            pollInput();

            scene->update(scene);

            if (ignoreMissedTicks) { // Ignore missed ticks if the ignoreMissedTicks flag is set. Reset flag
                ignoreMissedTicks = false;
                lag = 0;
            }

            if (!sceneStale && !Apricot_ASAP) { // Do not purge entities if scene is stale
                Scene_Purge(scene);
                lag -= Apricot_DeltaT;
                (*_ticks)++;
            } else {
                break;
            }
        }

        if (!sceneStale && !Apricot_ASAP) { // Do not render if scene is stale. Do not render if ASAP
            elapsedFrames = 0;
            SDL_SetRenderDrawColor(Apricot_Renderer, 21, 21, 21, 255);
            SDL_RenderClear(Apricot_Renderer);
            scene->render(scene);
            SDL_RenderPresent(Apricot_Renderer);
            frames++;
        }

        const Uint64 end = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();
        const double seconds = (end - start) / (float)(freq);
        if (seconds > 5.0) {
            // MUST be under 16,000 micro seconds
            printf("%d frames in %f seconds = %f FPS(%f us/frame), %f engine seconds\n", frames, seconds, frames / seconds, (seconds * 1000000.0) / frames, Apricot_Ticks / 60.0f);
            start = end;
            frames = 0;
        }
    }
}

/**
 * @brief Cleans up audio mixer and SDL
*/
static void apricotExit()
{
    Mix_CloseAudio();
    SDL_Quit();
}

/**
 * @brief Converts an SDL_KeyCode to a character corresponding to the shifted character on a US keyboard.
 * @param c SDL_KeyCode to convert
 * @return The corresponding shifted keycode
*/
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

/** 
 * @brief Polls and handles input from SDL event queue, updates game's state struct 
 */
static void pollInput()
{
    // PRE INPUT
    *_charDown = '\0';
    *_mouseLeftUp = false;
    *_mouseMoved = false;
    *_mouseRightUp = false;

    // DIRECT INPUT
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_WINDOWEVENT:
            SDL_GetWindowSize(Apricot_Window, &Apricot_Width, &Apricot_Height);
            break;
        case SDL_KEYDOWN:
            // Update chardown to be whichever char is pressed this tick
            if (Apricot_Keys[SDL_SCANCODE_UP] || Apricot_Keys[SDL_SCANCODE_DOWN] || Apricot_Keys[SDL_SCANCODE_LEFT] || Apricot_Keys[SDL_SCANCODE_RIGHT]) {
                *_charDown = APRICOT_ARROWKEY;
            } else if (Apricot_Keys[SDL_SCANCODE_LSHIFT] && event.key.keysym.scancode != SDL_SCANCODE_LSHIFT) {
                *_charDown = toshifted(SDL_GetKeyFromScancode(event.key.keysym.scancode));
            } else {
                *_charDown = SDL_GetKeyFromScancode(event.key.keysym.scancode);
            }

            _keys[event.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP:
            _keys[event.key.keysym.scancode] = false;
            break;
        case SDL_MOUSEMOTION: {
            *_mouseMoved = true;
            int x, y;
            SDL_GetMouseState(&x, &y);
            _mousePos->x = x;
            _mousePos->y = y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                *_mouseDrag = true;
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            Vector* temp = &Apricot_MouseWheel; // Bypass const
            temp->x += event.wheel.x;
            temp->y += event.wheel.y;
            break;
        }
        case SDL_MOUSEBUTTONDOWN: // Rising edge, not continuous
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                *_mouseLeftDown = true;
                int x, y;
                SDL_GetMouseState(&x, &y);
                _mouseInit->x = x;
                _mouseInit->y = y;
                break;
            case SDL_BUTTON_RIGHT:
                *_mouseRightDown = true;
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP: // Falling edge, not continuous
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                if (Apricot_MouseLeftDown) {
                    *_mouseLeftUp = true;
                }
                *_mouseLeftDown = false;
                *_mouseDrag = false;
                break;
            case SDL_BUTTON_RIGHT:
                if (Apricot_MouseRightDown) {
                    *_mouseRightUp = true;
                }
                *_mouseRightDown = false;
                break;
            }
            break;
        }
    }
}