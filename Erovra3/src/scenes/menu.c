/*	This scene encompasses the main menu, and the map form and preview.
 *	 
 *	Author: Joseph Shimel
 *	Date:	4/7/21
 */

#pragma once
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/font.h"
#include "../gui/gui.h"
#include "../terrain.h"
#include "../util/lexicon.h"
#include "match.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

EntityID loadingAssets;
EntityID mainMenu;
EntityID newGame;
EntityID newGameForm;
EntityID newGameActions;
EntityID loadingMatch;

EntityID mapSizeRadioButtons;
EntityID seaLevelSlider;
EntityID erosionSlider;
EntityID nationNameTextBox;
EntityID mapSeedTextBox;
EntityID AIControlledCheckBox;
EntityID terrainImage;
EntityID logoSpacer;
SDL_Texture* logo;

EntityID statusText;
EntityID progressBar;

EntityID loadingAssetsHints;
EntityID loadingAssetsImage;
SDL_Texture* loadingCircle = NULL;

// Pan position of the camera
Vector camera;
// Change in position over time
Vector vel;
// Change in velocity over time
Vector acc;

float logoSpacerVel;
float logoSpacerAcc;

// Preview map size
const int size = 4 * 64;
// Preview map data
float* map = NULL;
// Preview map texture
SDL_Texture* previewTexture = NULL;
// Swapped with previewTexture when updating preview
SDL_Texture* loading = NULL;

// Contains language data for nation name
Lexicon* lexicon;

enum state {
    GENERATING,
    EROSION
};
enum state state;
// Incremented by functions to asyncly get status for progress bar
int status = 0;
// Whether or not assets are loaded. Not reset after return to menu.
bool assetsLoaded = false;
// Set by generatePreview thread, map is updated, previewTexture needs to be repainted. Reset on menu return.
bool needsRepaint = false;
// Set by generateFullTerrain, full map is generating, show progress bar. Reset on menu return.
bool generating = false;
// Set by generatedFullTerrain, full map is done, start match. Reset on menu return.
bool done = false;

/*	Calculates seed based on seed text box
* 
*	@param scene	Main menu scene
*/
static int getSeed(Scene* scene)
{
    TextBox* seedBox = (TextBox*)Scene_GetComponent(scene, mapSeedTextBox, GUI_TEXT_BOX_COMPONENT_ID);
    unsigned int seed = 5381;
    for (int i = 0; i < 32 && seedBox->text[i] != '\0'; i++) {
        seed = seed + seedBox->text[i] * 33;
    }
    return seed;
}

/*	Loads in the lexicon. Flips the assetsLoaded flag to true when done.
 *
 *	@param scene	This should be a pointer to the current scene
 */
static int loadAssets(Scene* scene)
{
    status = 0;
    lexicon = Lexicon_Create("res/countryNames.txt", &status);
    assetsLoaded = true;
    return 0;
}

/*	A thread that is called whenever a map setting is modified by the user. 
*	This function differs from generateFullTerrain because it uses the smaller
*	size parameter, and sets the needsRepaint flag.
* 
*	@param ptr	This should be a pointer to the current scene
*/
static int generatePreview(void* ptr)
{
    Scene* scene = (Scene*)ptr;
    free(map);

    // Generate map
    status = 0;
    state = GENERATING;
    map = terrain_perlin(size, size / 4, getSeed(scene), &status);
    Slider* seaLevel = (Slider*)Scene_GetComponent(scene, seaLevelSlider, GUI_SLIDER_COMPONENT_ID);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            map[x + y * size] = map[x + y * size] * 0.5f + (1.0f - seaLevel->value) * 0.5f;
        }
    }

    // Do erosion
    Slider* erosion = (Slider*)Scene_GetComponent(scene, erosionSlider, GUI_SLIDER_COMPONENT_ID);
    status = 0;
    state = EROSION;
    terrain_erode(size, map, erosion->value * 3, &status);

    // Set needsRepaint flag. Menu_Update() will monitor this flag and repaint texutre, and unset this flag.
    needsRepaint = 1;
    return 0;
}

/*	Thread function, generates map for match. Sets the done flag when done.
 *	
 *	@param ptr	 A pointer to a Scene structure */
static int generateFullTerrain(void* ptr)
{
    Scene* scene = (Scene*)ptr;

    RadioButtons* mapSize = (RadioButtons*)Scene_GetComponent(scene, mapSizeRadioButtons, GUI_RADIO_BUTTONS_COMPONENT_ID);
    int fullMapSize = 8 * (int)pow(2, mapSize->selection) * 64;
    Slider* seaLevel = (Slider*)Scene_GetComponent(scene, seaLevelSlider, GUI_SLIDER_COMPONENT_ID);
    Slider* erosion = (Slider*)Scene_GetComponent(scene, erosionSlider, GUI_SLIDER_COMPONENT_ID);

    /* Generate map */
    free(map);
    status = 0;
    state = GENERATING;
    // pass status integer, is incremented by terrain_perlin(). Used by update function for progress bar
    map = terrain_perlin(fullMapSize, fullMapSize / 4, getSeed(scene), &status);
    for (int y = 0; y < fullMapSize; y++) {
        for (int x = 0; x < fullMapSize; x++) {
            map[x + y * fullMapSize] = map[x + y * fullMapSize] * 0.5f + (1.0f - seaLevel->value) * 0.5f;
        }
    }

    /* Erode map */
    status = 0;
    state = EROSION;
    // pass status integer, is incremented by terrain_perlin(). Used by update function for progress bar
    terrain_erode(fullMapSize, map, erosion->value * 3, &status);

    // Set done flag to true. Update monitors done flag, will call Match_Init() when map is finished
    done = true;
    return 0;
}

/*	Regenerates the map preview whenever an update to the GUI controls has been made.
 *
 *	@param scene	The main menu scene
 *	@param id		The EntityID of the GUI object that invokes this function. Unused.
 */
void Menu_ReconstructMap(Scene* scene, EntityID id)
{
    // Copy current preview texture to loading texture
    SDL_SetRenderTarget(g->rend, loading);
    SDL_RenderCopy(g->rend, previewTexture, NULL, NULL);
    // Darken loading texture a bit, return renderer to window
    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, 50);
    SDL_RenderFillRect(g->rend, NULL);
    SDL_SetRenderTarget(g->rend, NULL);

    // Set map preview image entity to display the loading image
    Image* terrain = (Image*)Scene_GetComponent(scene, terrainImage, GUI_IMAGE_COMPONENT_ID);
    terrain->texture = loading;
    // (will be reset back to new previewTexture once the following thread finishes updating the map)

    // Call the generatePreview to update the map
    SDL_Thread* thread = SDL_CreateThread(generatePreview, "Generate preview", scene);

    // Resume updating GUI
}

/*	Called by the "Start new game" and "Randomize" buttons. Sets name and seed 
 *	to random values, and reconstructs map.
 *
 *	@param scene	Pointer to main menu scene struct
 *	@param id		The EntityID of the button that called callback (Unused)
 */
void Menu_RandomizeValues(Scene* scene, EntityID id)
{
    TextBox* nationName = (TextBox*)Scene_GetComponent(scene, nationNameTextBox, GUI_TEXT_BOX_COMPONENT_ID);
    TextBox* mapSeed = (TextBox*)Scene_GetComponent(scene, mapSeedTextBox, GUI_TEXT_BOX_COMPONENT_ID);
    Slider* seaLevel = (Slider*)Scene_GetComponent(scene, seaLevelSlider, GUI_SLIDER_COMPONENT_ID);
    Slider* erosion = (Slider*)Scene_GetComponent(scene, erosionSlider, GUI_SLIDER_COMPONENT_ID);

    /* Reset slider positions */
    seaLevel->value = 0.33f;
    erosion->value = 0.0f;

    /* Randomize name */
    char randName[32];
    Lexicon_GenerateWord(lexicon, randName, 10);
    strncpy_s(nationName->text, 32, randName, 32);
    nationName->length = (int)strlen(randName);

    /* Randomize seed */
    char randSeed[32];
    srand((unsigned)time(0));
    for (int i = 0; i < 32; i++) {
        randSeed[i] = rand() % 26 + 'a';
    }
    randSeed[31] = '\0';
    strncpy_s(mapSeed->text, 32, randSeed, 32);
    mapSeed->length = 32;

    Menu_ReconstructMap(scene, id);
}

/*	Pans camera from home screen to map form, resets values
 *
 *	@param scene	Pointer to main menu scene struct
 *	@param id		EntityID of button (unused)
 */
void Menu_GotoMapForm(Scene* scene, EntityID id)
{
    acc.x = 20;
    Menu_RandomizeValues(scene, id);
}

/*	Pans camera back from map form to home screen
 *
 *	@param scene	Pointer to main menu scene struct (unused)
 *	@param id		EntityID of button (unused)
 */
void Menu_Back(Scene* scene, EntityID id)
{
    acc.x = -20;
}

/*	Invokes generateFullTerrain thread, sets generating flag to true
 *
 *	@param scene	Pointer to main menu scene struct (unused)
 *	@param id		EntityID of button (unused)
 */
void Menu_StartMatch(Scene* scene, EntityID id)
{
    generating = true; // Means that game is generating match map, pan to progress bar
    SDL_Thread* thread = SDL_CreateThread(generateFullTerrain, "Generate full terrain", scene);
    camera.x = -2000;
    camera.y = -2000;
}

/*	Ends the game 
 * 
 *	@param scene	Pointer to main menu scene struct (unused)
 *	@param id		EntityID of button (unused) 
 */
void Menu_Exit(Scene* scene, EntityID id)
{
    exit(0);
}

/*	Called every tick. Handles camera velocity and acceleration and GUI 
 *	containers positions on screen, and updates the progress bar and status
 *	text.
 *
 *	@param scene	Pointer to main menu scene struct
 */
void Menu_Update(Scene* scene)
{
    // Check to see if the main menu is currently loading in assets
    if (!assetsLoaded) {
        Image* image = (Image*)Scene_GetComponent(scene, loadingAssetsImage, GUI_IMAGE_COMPONENT_ID);
        image->angle += 6.28f;
    }
    // Check to see if the main menu is currently generating a full map
    else if (generating) {
        // Check if the generateFullMatch thread has finished generating the map
        if (done) {
            RadioButtons* mapSize = (RadioButtons*)Scene_GetComponent(scene, mapSizeRadioButtons, GUI_RADIO_BUTTONS_COMPONENT_ID);
            CheckBox* AIControlled = (CheckBox*)Scene_GetComponent(scene, AIControlledCheckBox, GUI_CHECK_BOX_COMPONENT_ID);
            int fullMapSize = 8 * (int)pow(2, mapSize->selection) * 64;

            // Setup a new texture, call Match_Init, start game!
            SDL_Texture* texture = SDL_CreateTexture(g->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, fullMapSize, fullMapSize);
            paintMap(fullMapSize, map, texture);

            // Reset all values to defaults before calling into Match
            camera.x = -4000;
            camera.y = -4000;
            status = 0;
            needsRepaint = false;
            generating = false;
            done = false;

            Match_Init(fullMapSize, map, texture, AIControlled->value);
            return; // Always return after scene stack disruption!
        } else {
            RadioButtons* mapSize = (RadioButtons*)Scene_GetComponent(scene, mapSizeRadioButtons, GUI_RADIO_BUTTONS_COMPONENT_ID);
            Slider* erosion = (Slider*)Scene_GetComponent(scene, erosionSlider, GUI_SLIDER_COMPONENT_ID);
            ProgressBar* progress = (ProgressBar*)Scene_GetComponent(scene, progressBar, GUI_PROGRESS_BAR_COMPONENT_ID);
            Label* label = (Label*)Scene_GetComponent(scene, statusText, GUI_LABEL_ID);

            int fullMapSize = 8 * (int)pow(2, mapSize->selection) * 64;

            // Calculate progress bar max status counts based on state
            double denominator = 1;
            if (state == GENERATING) {
                denominator = 5.0f;
                strncpy_s(label->text, 32, "Generating...", 32);
            } else if (state == EROSION) {
                strncpy_s(label->text, 32, "Eroding...", 32);
                denominator = (double)(fullMapSize * fullMapSize * erosion->value * 3);
            }
            progress->value = (float)((double)status / denominator);
        }
    } else {
        // needsRepaint flag is called by generatePreview thread
        // Signifies that preview map is updated and the preview needs to be redrawn
        if (needsRepaint) {
            paintMap(size, map, previewTexture);
            needsRepaint = false;
            Image* terrain = (Image*)Scene_GetComponent(scene, terrainImage, GUI_IMAGE_COMPONENT_ID);
            terrain->texture = previewTexture;
        }

        GUIComponent* logoSpacerGUI = (GUIComponent*)Scene_GetComponent(scene, logoSpacer, GUI_COMPONENT_ID);
        logoSpacerVel += logoSpacerAcc;
        logoSpacerGUI->height += logoSpacerVel;
        logoSpacerAcc *= 0.9f;
        logoSpacerVel *= 0.9f;

        /* Update camera physics */
        vel = Vector_Add(vel, acc);
        camera = Vector_Add(camera, vel);
        acc = Vector_Scalar(acc, 0.9f);
        vel = Vector_Scalar(vel, 0.9f);
        if (camera.x < -2000) {
            camera.x = 0;
            camera.y = 0;
        }

        GUI_Update(scene);
    }

    /* Update container's position relative to camera */
    GUI_CenterElementAt(scene, mainMenu, -camera.x, -camera.y);
    GUI_CenterElementAt(scene, newGame, -camera.x + 2000, -camera.y);
    GUI_CenterElementAt(scene, loadingMatch, -camera.x - 2000, -camera.y - 2000);
    GUI_CenterElementAt(scene, loadingAssets, -camera.x - 4000, -camera.y - 3876);
}

/*	Calls GUI draw function.
 *
 *	@param scene	Pointer to main menu scene struct
 */
void Menu_Render(Scene* scene)
{
    GUI_Render(scene);
}

/*	Called when game starts. Creates the main menu GUI objects and the main menu
 *	scene struct.
 */
Scene* Menu_Init()
{
    Scene* scene = Scene_Create(GUI_Register, &Menu_Update, &Menu_Render, NULL);

    logo = loadTexture("res/logo.png");
    loading = loadTexture("res/loading.png");
    loadingCircle = loadTexture("res/loadingCircle.png");

    SDL_Thread* assetLoadingThread = SDL_CreateThread(loadAssets, "Load assets", scene);
    previewTexture = SDL_CreateTexture(g->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, size, size);

    camera = (Vector) { -4000, -4000 };
    vel = (Vector) { 0, 0 };
    acc.y = 0;

    logoSpacerVel = 0;
    logoSpacerAcc = -20;

    mapSizeRadioButtons = GUI_CreateRadioButtons(scene, (Vector) { 0, 0 }, "Map size", 1, 3, "Small (8x8)", "Medium (16x16)", "Large (32x32)");
    seaLevelSlider = GUI_CreateSlider(scene, (Vector) { 0, 0 }, 280, "Sea level", 0.33f, &Menu_ReconstructMap);
    erosionSlider = GUI_CreateSlider(scene, (Vector) { 0, 0 }, 280, "Erosion", 0.5f, &Menu_ReconstructMap);
    nationNameTextBox = GUI_CreateTextBox(scene, (Vector) { 0, 0 }, 280, "Nation name", "", NULL);
    mapSeedTextBox = GUI_CreateTextBox(scene, (Vector) { 0, 0 }, 280, "Map seed", "", &Menu_ReconstructMap);
    AIControlledCheckBox = GUI_CreateCheckBox(scene, (Vector) { 0, 0 }, "AI controlled", false);
    terrainImage = GUI_CreateImage(scene, (Vector) { 0, 0 }, 447, 447, previewTexture);

    statusText = GUI_CreateLabel(scene, (Vector) { 0, 0 }, "Um, lol?");
    progressBar = GUI_CreateProgressBar(scene, (Vector) { 0, 0 }, 840, 0.7f);

    loadingAssetsHints = GUI_CreateLabel(scene, (Vector) { 0, 0 }, "You can click on units to click on them!");
    loadingAssetsImage = GUI_CreateImage(scene, (Vector) { 0, 0 }, 50, 50, loadingCircle);

    logoSpacer = GUI_CreateSpacer(scene, (Vector) { 0, 0 }, 0, 2050);

    loadingAssets = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, loadingAssets, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, loadingAssets, loadingAssetsImage);
    GUI_ContainerAdd(scene, loadingAssets, GUI_CreateSpacer(scene, (Vector) { 0, 0 }, 0, 204));
    GUI_ContainerAdd(scene, loadingAssets, loadingAssetsHints);

    mainMenu = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, mainMenu, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_SetBackgroundColor(scene, mainMenu, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateImage(scene, (Vector) { 0, 0 }, 641, 141, logo));
    GUI_ContainerAdd(scene, mainMenu, logoSpacer);
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Start New Game", 0, &Menu_GotoMapForm));
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Report a Bug", 0, NULL));
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, &Menu_Exit));

    newGameForm = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 450);
    GUI_SetBackgroundColor(scene, newGameForm, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, newGameForm, mapSizeRadioButtons);
    GUI_ContainerAdd(scene, newGameForm, seaLevelSlider);
    GUI_ContainerAdd(scene, newGameForm, erosionSlider);
    GUI_ContainerAdd(scene, newGameForm, nationNameTextBox);
    GUI_ContainerAdd(scene, newGameForm, mapSeedTextBox);
    GUI_ContainerAdd(scene, newGameForm, AIControlledCheckBox);
    GUI_ContainerAdd(scene, newGameForm, terrainImage);

    newGameActions = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 51);
    GUI_SetBackgroundColor(scene, newGameActions, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, newGameActions, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Back", 0, &Menu_Back));
    GUI_ContainerAdd(scene, newGameActions, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Randomize", 0, &Menu_RandomizeValues));
    GUI_ContainerAdd(scene, newGameActions, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Start!", 0, &Menu_StartMatch));

    newGame = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, newGame, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, newGame, newGameForm);
    GUI_ContainerAdd(scene, newGame, newGameActions);

    loadingMatch = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    Scene_Assign(scene, loadingMatch, GUI_CENTERED_COMPONENT_ID, NULL);
    GUI_ContainerAdd(scene, loadingMatch, GUI_CreateLabel(scene, (Vector) { 0, 0 }, "Loading match"));
    GUI_ContainerAdd(scene, loadingMatch, statusText);
    GUI_ContainerAdd(scene, loadingMatch, progressBar);

    Game_PushScene(scene);
    return scene;
}