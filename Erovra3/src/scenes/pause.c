#include "pause.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "../util/debug.h"
#include "./match.h"

static Scene* matchScene = NULL;
int fade = 0;

void Pause_Update(Scene* scene)
{
    static escDown = true;
    if (g->keys[SDL_SCANCODE_ESCAPE]) {
        if (!escDown) {
            Game_PopScene(1);
            escDown = true;
            return;
        }
        escDown = true;
    } else {
        escDown = false;
    }
    if (fade < 128) {
        fade += 128 / 10;
    }
}

void Pause_Render(Scene* scene)
{
    Match_Render(matchScene);
    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, fade);
    SDL_RenderFillRect(g->rend, NULL);

    SDL_Rect rect = { g->width / 2 - 50, g->height / 2 - 50 + (128.0f - fade) * 1080 / 128.0f, 100, 100 };
    SDL_SetRenderDrawColor(g->rend, 255, 0, 0, 255);
    SDL_RenderFillRect(g->rend, &rect);
}

void Pause_Destroy(Scene* scene)
{
    GUI_Destroy(scene);
}

Scene* Pause_Init(Scene* mScene)
{
    Scene* scene = Scene_Create(&GUI_Register, &Pause_Update, &Pause_Render, &Pause_Destroy);
    matchScene = mScene;
    fade = 0;

    Game_PushScene(scene);
    return scene;
}