#pragma once
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "../terrain.h"
#include "match.h"
#include <stdio.h>

EntityID mainMenu;
EntityID newGame;
EntityID newGameForm;
EntityID newGameActions;

EntityID seaLevelSlider;
EntityID erosionSlider;
EntityID mapSeedTextBox;

Vector camera;
Vector vel;
Vector acc;

const int size = 3 * 64;
float* map = NULL;
SDL_Texture* texture = NULL;

bool firstUpdate = true;

void Menu_RepaintMap(Scene* scene, EntityID id)
{
    free(map);

    TextBox* seedBox = (TextBox*)Scene_GetComponent(scene, mapSeedTextBox, GUI_TEXT_BOX_COMPONENT_ID);
    unsigned int seed = 5381;
    for (int i = 0; i < 32 && seedBox->text[i] != '\0'; i++) {
        seed = seed + seedBox->text[i] * 33;
    }
    map = terrain_perlin(size, size / 4.0f, seed);

    Slider* seaLevel = (Slider*)Scene_GetComponent(scene, seaLevelSlider, GUI_SLIDER_COMPONENT_ID);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            map[x + y * size] = map[x + y * size] * 0.5 + (1.0f - seaLevel->value) * 0.5f; // islands
        }
    }
    Slider* erosion = (Slider*)Scene_GetComponent(scene, erosionSlider, GUI_SLIDER_COMPONENT_ID);
    terrain_erode(size, map, erosion->value * 3);
    paintMap(size, map, texture);
    printf("Done.\n");
}

/*
	Starts a new game */
void Menu_GotoMapForm(Scene* scene, EntityID id)
{
    //Match_Init();
    acc.x = 12; // 960 pixels, 240 bpm
    Menu_RepaintMap(scene, id);
}

/*
	Starts a new game */
void Menu_Back()
{
    acc.x = -12; // 960 pixels, 240 bpm
}

void Menu_StartMatch(Scene* scene, EntityID id)
{
    TextBox* seedBox = (TextBox*)Scene_GetComponent(scene, mapSeedTextBox, GUI_TEXT_BOX_COMPONENT_ID);
    Slider* seaLevel = (Slider*)Scene_GetComponent(scene, seaLevelSlider, GUI_SLIDER_COMPONENT_ID);
    Slider* erosion = (Slider*)Scene_GetComponent(scene, erosionSlider, GUI_SLIDER_COMPONENT_ID);
    unsigned int seed = 5381;
    for (int i = 0; i < 32 && seedBox->text[i] != '\0'; i++) {
        seed = seed + seedBox->text[i] * 33;
    }

    Match_Init(12, (1.0f - seaLevel->value) * 0.5f, seed, erosion->value * 3);
}

/*
	Called every tick */
void Menu_Update(Scene* scene)
{
    vel = Vector_Add(vel, acc);
    camera = Vector_Add(camera, vel);
    acc = Vector_Scalar(acc, 0.9);
    vel = Vector_Scalar(vel, 0.9);

    GUIComponent* mainMenuGUI = (GUIComponent*)Scene_GetComponent(scene, mainMenu, GUI_COMPONENT_ID);
    mainMenuGUI->pos.x = g->width / 2 - 350 / 2 - camera.x;
    mainMenuGUI->pos.y = g->height / 2 - 87 - camera.y;
    GUI_SetContainerShown(scene, mainMenu, true);

    GUIComponent* newGameGUI = (GUIComponent*)Scene_GetComponent(scene, newGame, GUI_COMPONENT_ID);
    Container* newGameContainer = (Container*)Scene_GetComponent(scene, newGame, GUI_CONTAINER_COMPONENT_ID);
    newGameGUI->pos.x = g->width / 2 - newGameGUI->width / 2 - camera.x + 1200;
    newGameGUI->pos.y = g->height / 2 - newGameGUI->height / 2 - camera.y;
    GUI_SetContainerShown(scene, newGame, true);

    GUI_Update(scene);
}

/*
	Draws every screen draw */
void Menu_Render(Scene* scene)
{
    GUI_Render(scene);
}

/*
	Called when the menu starts */
Scene* Menu_Init()
{
    Scene* scene = Scene_Create(Components_Init, &Menu_Update, &Menu_Render);
    GUI_Init(scene);

    camera = (Vector) { 0, 0 };
    vel = (Vector) { 0, 0 };
    acc.y = 0;

    texture = SDL_CreateTexture(g->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, size, size);

    seaLevelSlider = GUI_CreateSlider(scene, (Vector) { 0, 0 }, 280, "Sea level", 0.33f, &Menu_RepaintMap);
    erosionSlider = GUI_CreateSlider(scene, (Vector) { 0, 0 }, 280, "Erosion", 0.5f, &Menu_RepaintMap);
    char randSeed[32];
    srand((unsigned)time(0));
    for (int i = 0; i < 32; i++) {
        randSeed[i] = rand() % 26 + 'a';
    }
    randSeed[31] = '\0';
    mapSeedTextBox = GUI_CreateTextBox(scene, (Vector) { 0, 0 }, 280, "Map seed", randSeed, &Menu_RepaintMap);

    mainMenu = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    GUI_SetBackgroundColor(scene, mainMenu, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Start New Game", 0, &Menu_GotoMapForm));
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Report a Bug", 0, NULL));
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, NULL));

    newGameForm = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 450);
    GUI_SetBackgroundColor(scene, newGameForm, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, newGameForm, GUI_CreateRadioButtons(scene, (Vector) { 0, 0 }, "Map size", 1, 3, "Small (8x8)", "Medium (16x16)", "Large (32x32)"));
    GUI_ContainerAdd(scene, newGameForm, seaLevelSlider);
    GUI_ContainerAdd(scene, newGameForm, erosionSlider);
    GUI_ContainerAdd(scene, newGameForm, GUI_CreateTextBox(scene, (Vector) { 0, 0 }, 280, "Nation name", "", NULL));
    GUI_ContainerAdd(scene, newGameForm, mapSeedTextBox);
    GUI_ContainerAdd(scene, newGameForm, GUI_CreateCheckBox(scene, (Vector) { 0, 0 }, "AI controlled", false));
    GUI_ContainerAdd(scene, newGameForm, GUI_CreateImage(scene, (Vector) { 0, 0 }, 447, 447, texture));

    newGameActions = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 51);
    GUI_SetBackgroundColor(scene, newGameActions, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, newGameActions, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Back", 0, &Menu_Back));
    GUI_ContainerAdd(scene, newGameActions, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Start!", 0, &Menu_StartMatch));

    newGame = GUI_CreateContainer(scene, (Vector) { 0, 0 }, 1080);
    GUI_ContainerAdd(scene, newGame, newGameForm);
    GUI_ContainerAdd(scene, newGame, newGameActions);

    Game_PushScene(scene);
    return scene;
}