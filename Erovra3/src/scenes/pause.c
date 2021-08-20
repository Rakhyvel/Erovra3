#include "pause.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../util/debug.h"
#include "./match.h"

static Scene* matchScene = NULL;

void Pause_Components(Scene* scene)
{
}

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
}

void Pause_Render(Scene* scene)
{
    Match_Render(matchScene);
    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, 128);
    SDL_RenderFillRect(g->rend, NULL);
}

Scene* Pause_Init(Scene* mScene)
{
    Scene* scene = Scene_Create(&Pause_Components, &Pause_Update, &Pause_Render, NULL);
    matchScene = mScene;

    Game_PushScene(scene);
    return scene;
}