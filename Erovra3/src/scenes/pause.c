#include "pause.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "../util/debug.h"
#include "./match.h"

static Scene* matchScene = NULL;

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

void Pause_Destroy(Scene* scene)
{
    GUI_Destroy(scene);
}

Scene* Pause_Init(Scene* mScene)
{
    Scene* scene = Scene_Create(&GUI_Register, &Pause_Update, &Pause_Render, &Pause_Destroy);
    matchScene = mScene;

    Game_PushScene(scene);
    return scene;
}