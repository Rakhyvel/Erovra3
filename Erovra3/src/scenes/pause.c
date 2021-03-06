#include "pause.h"
#include "../assemblages/components.h"
#include "../engine/apricot.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "../util/debug.h"
#include "./match.h"
#include "./menu.h"

/* GUI containers */
EntityID pauseMenuContainer;
EntityID victoryMenuContainer;
EntityID defeatMenuContainer;

/* GUI images for splash */
EntityID victoryImage;
EntityID defeatImage;
SDL_Texture* victoryTexture = NULL;
SDL_Texture* defeatTexture = NULL;

/* The pauseState the pause menu is currently in */
enum pauseState pauseState;

/* Reference to the match scene. Used to render match in background */
static Scene* matchScene = NULL;

/* Transition value */
int fade = 0;

/*	Callback function, called from "surrender" button in pause menu. Ends game 
 *	in defeat.
 * 
 *	@param scene	Scene reference (unused)
 *	@param id		EntityID of button that trigged callback (unused)
 */
void Pause_Surrender(Scene* scene, EntityID id)
{
    pauseState = DEFEAT;
}

/*	Callback function, called from "back to game" button in pause menu. 
 *	Returns to match.
 * 
 *	@param scene	Scene reference (unused)
 *	@param id		EntityID of button that trigged callback (unused)
 */
void Pause_BackToGame(Scene* scene, EntityID id)
{
    pauseState = RETURN_MATCH;
}

/*	Callback function, called from "back to menu" button in pause menus. Returns
 *	to main menu.
 * 
 *	@param scene	Scene reference (unused)
 *	@param id		EntityID of button that trigged callback (unused)
 */
void Pause_BackToMenu(Scene* scene, EntityID id)
{
    pauseState = RETURN_MENU;
}

/*	Callback function, called from "exit" button in pause menus. Ends game.
 * 
 *	@param scene	Scene reference (unused)
 *	@param id		EntityID of button that trigged callback (unused)
 */
void Pause_Exit(Scene* scene, EntityID id)
{
    exit(0);
}

/*	Checks menu pauseState, and increments/decrements fade transition value accordingly
 * 
 *	@param scene	Scene reference
 */
void Pause_Update(Scene* scene)
{
    static escDown = true;
	// Regular menus
    if (pauseState == PAUSE || pauseState == VICTORY || pauseState == DEFEAT) {
		// If in pause scene, check if esc key is pressed. Return to match if so
        if (pauseState == PAUSE && Apricot_Keys[SDL_SCANCODE_ESCAPE]) {
            if (!escDown) {
                pauseState = RETURN_MATCH;
                escDown = true;
                return;
            }
            escDown = true;
        } else {
            escDown = false;
        }

		// Update transition value
        if (fade < 128) {
            fade += 128 / 10;
        }
    } 
	// Transition pauseStates RETURN_MATCH and RETURN_MENU. Update transistion pauseStates
	else if (fade > 0) {
        fade -= 128 / 10;
    } 
	// Finally execute actions
	else {
        // Reset statics to default before leaving
        escDown = true;
        if (pauseState == RETURN_MATCH) {
            Apricot_PopScene(1);
        } else if (pauseState == RETURN_MENU) {
            Apricot_PopScene(2);
        }
        return;
    }

	// Translate containers and images according to fade value
    GUI_CenterElementAt(scene, pauseMenuContainer, 0, pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, victoryMenuContainer, 0, pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, defeatMenuContainer, 0, pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, victoryImage, 0, -150 - pow((128.0f - fade) / 128.0f, 2) * 1000);
    GUI_CenterElementAt(scene, defeatImage, 0, -150 - pow((128.0f - fade) / 128.0f, 2) * 1000);

	// Select which container and image to show based on pauseState
    GUI_SetShown(scene, pauseMenuContainer, pauseState == PAUSE);
    GUI_SetShown(scene, victoryMenuContainer, pauseState == VICTORY);
    GUI_SetShown(scene, victoryImage, pauseState == VICTORY);
    GUI_SetShown(scene, defeatMenuContainer, pauseState == DEFEAT);
    GUI_SetShown(scene, defeatImage, pauseState == DEFEAT);

    GUI_Update(scene);
}

/*	Calls match render system, with a translucent overlay accoridng to the fade,
 *	and finally draws the menus. 
 * 
 *	@param scene	Scene reference
 */
void Pause_Render(Scene* scene)
{
    Match_Render(matchScene);
    SDL_SetRenderDrawColor(Apricot_Renderer, 0, 0, 0, fade);
    SDL_RenderFillRect(Apricot_Renderer, NULL);
    GUI_Render(scene);
}

/*	Destroys scene
 * 
 *	@param scene	Scene reference to destroy
 */
void Pause_Destroy(Scene* scene)
{
    GUI_Destroy(scene);
}

/*	Creates scene
 * 
 *	@param scene	Scene reference
 *	@param s		Pause pauseState to start pause scene in
 */
Scene* Pause_Init(Scene* mScene, enum pauseState s)
{
    Scene* scene = Scene_Create(&GUI_Register, &Pause_Update, &Pause_Render, &Pause_Destroy);
    matchScene = mScene;
    fade = 0;
    pauseState = s;

    if (!victoryTexture) {
        victoryTexture = Texture_Load("res/victory.png");
    }
    if (!defeatTexture) {
        defeatTexture = Texture_Load("res/defeat.png");
    }

    victoryImage = GUI_CreateImage(scene, (Vector) { 0, 0 }, 619 * 0.75, 101 * 0.75, victoryTexture);
    defeatImage = GUI_CreateImage(scene, (Vector) { 0, 0 }, 532 * 0.75, 101 * 0.75, defeatTexture);

    pauseMenuContainer = GUI_CreateContainer(scene, (Vector) { 0, 0 }, -1, -1);
    Scene_Assign(scene, pauseMenuContainer, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, pauseMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Back to game", 0, &Pause_BackToGame));
    GUI_ContainerAdd(scene, pauseMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Settings", 0, NULL));
    GUI_ContainerAdd(scene, pauseMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Surrender", 0, &Pause_Surrender));

    victoryMenuContainer = GUI_CreateContainer(scene, (Vector) { 0, 0 }, -1, -1);
    Scene_Assign(scene, victoryMenuContainer, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, victoryMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Main menu", 0, &Pause_BackToMenu));
    GUI_ContainerAdd(scene, victoryMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, &Pause_Exit));

    defeatMenuContainer = GUI_CreateContainer(scene, (Vector) { 0, 0 }, -1, -1);
    Scene_Assign(scene, defeatMenuContainer, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, defeatMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Main menu", 0, &Pause_BackToMenu));
    GUI_ContainerAdd(scene, defeatMenuContainer, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, &Pause_Exit));

    Apricot_PushScene(scene);
    return scene;
}