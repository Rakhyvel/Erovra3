#include "pause.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "../util/debug.h"
#include "./match.h"
#include "./menu.h"

EntityID pauseMenuContainer;
EntityID victoryMenuContainer;
EntityID defeatMenuContainer;

EntityID victoryImage;
EntityID defeatImage;

SDL_Texture* victoryTexture = NULL;
SDL_Texture* defeatTexture = NULL;

enum pauseState state;

static Scene* matchScene = NULL;
int fade = 0;

void Pause_Surrender(Scene* scene, EntityID id)
{
    state = DEFEAT;
}

void Pause_BackToGame(Scene* scene, EntityID id)
{
    state = RETURN_MATCH;
}

void Pause_BackToMenu(Scene* scene, EntityID id)
{
    state = RETURN_MENU;
}

void Pause_Exit(Scene* scene, EntityID id)
{
    exit(0);
}

void Pause_Update(Scene* scene)
{
    static escDown = true;
    if (state == PAUSE || state == VICTORY || state == DEFEAT) {
        if (state == PAUSE && g->keys[SDL_SCANCODE_ESCAPE]) {
            if (!escDown) {
                state = RETURN_MATCH;
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
    } else if (fade > 0) {
        fade -= 128 / 10;
    } else {
        // Reset statics to default before leaving
        escDown = true;
        if (state == RETURN_MATCH) {
            Game_PopScene(1);
        } else if (state == RETURN_MENU) {
            Game_PopScene(2);
        }
        return;
    }

    GUI_CenterElementAt(scene, pauseMenuContainer, 0, pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, victoryMenuContainer, 0, pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, defeatMenuContainer, 0, pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, victoryImage, 0, -150 - pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, defeatImage, 0, -150 - pow((128.0f - fade) / 128.0f, 2) * 1000);

    GUI_SetShown(scene, pauseMenuContainer, state == PAUSE);
    GUI_SetShown(scene, victoryMenuContainer, state == VICTORY);
    GUI_SetShown(scene, victoryImage, state == VICTORY);
    GUI_SetShown(scene, defeatMenuContainer, state == DEFEAT);
    GUI_SetShown(scene, defeatImage, state == DEFEAT);

    GUI_Update(scene);
}

void Pause_Render(Scene* scene)
{
    Match_Render(matchScene);
    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, fade);
    SDL_RenderFillRect(g->rend, NULL);
    GUI_Render(scene);
}

void Pause_Destroy(Scene* scene)
{
    GUI_Destroy(scene);
}

Scene* Pause_Init(Scene* mScene, enum pauseState s)
{
    Scene* scene = Scene_Create(&GUI_Register, &Pause_Update, &Pause_Render, &Pause_Destroy);
    matchScene = mScene;
    fade = 0;
    state = s;

    if (!victoryTexture) {
        victoryTexture = loadTexture("res/victory.png");
    }
    if (!defeatTexture) {
        defeatTexture = loadTexture("res/defeat.png");
    }

    victoryImage = GUI_CreateImage(scene, (Vector) { 0, 0 }, 619 * 0.75, 101 * 0.75, victoryTexture);
    defeatImage = GUI_CreateImage(scene, (Vector) { 0, 0 }, 532 * 0.75, 101 * 0.75, defeatTexture);

    pauseMenuContainer = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, pauseMenuContainer, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, pauseMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Surrender", 0, &Pause_Surrender));
    GUI_ContainerAdd(scene, pauseMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Settings", 0, NULL));
    GUI_ContainerAdd(scene, pauseMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Back to game", 0, &Pause_BackToGame));

    victoryMenuContainer = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, victoryMenuContainer, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, victoryMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Main menu", 0, &Pause_BackToMenu));
    GUI_ContainerAdd(scene, victoryMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, &Pause_Exit));

    defeatMenuContainer = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, defeatMenuContainer, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, defeatMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Main menu", 0, &Pause_BackToMenu));
    GUI_ContainerAdd(scene, defeatMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, &Pause_Exit));

    Game_PushScene(scene);
    return scene;
}