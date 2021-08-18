#pragma once
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "match.h"
#include <stdio.h>

EntityID mainMenu;
EntityID newGame;

Vector camera;
Vector vel;
Vector acc;

/*
	Starts a new game */
void Menu_StartNewGame()
{
    //Match_Init();
    acc.x = 12; // 960 pixels, 240 bpm
}
/*
	Starts a new game */
void Menu_Back()
{
    //Match_Init();
    acc.x = -12; // 960 pixels, 240 bpm
}

void Menu_SeaLevelCallback(Scene* scene, EntityID id)
{
    Slider* slider = (Slider*)Scene_GetComponent(scene, id, GUI_SLIDER_COMPONENT_ID);
    printf("%f\n", slider->value);
}

void Menu_ErosionCallback(Scene* scene, EntityID id)
{
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
    newGameGUI->pos.x = g->width / 2 - 350 / 2 - camera.x + 1200;
    newGameGUI->pos.y = g->height / 2 - 280 - camera.y;
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

    mainMenu = GUI_CreateContainer(scene, (Vector) { 0, 0 });
    GUI_SetBackgroundColor(scene, mainMenu, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Start New Game", 0, &Menu_StartNewGame));
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Report a Bug", 0, NULL));
    GUI_ContainerAdd(scene, mainMenu, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Exit", 0, NULL));

    newGame = GUI_CreateContainer(scene, (Vector) { 0, 0 });
    GUI_SetBackgroundColor(scene, newGame, (SDL_Color) { 0, 0, 0, 0 });
    GUI_ContainerAdd(scene, newGame, GUI_CreateRadioButtons(scene, (Vector) { 0, 0 }, "Map size", 1, 3, "Small (8x8)", "Medium (16x16)", "Large (32x32)"));
    GUI_ContainerAdd(scene, newGame, GUI_CreateSlider(scene, (Vector) { 0, 0 }, 280, "Sea level", 0.75f, &Menu_SeaLevelCallback));
    GUI_ContainerAdd(scene, newGame, GUI_CreateSlider(scene, (Vector) { 0, 0 }, 280, "Erosion", 0.75f, &Menu_ErosionCallback));
    GUI_ContainerAdd(scene, newGame, GUI_CreateTextBox(scene, (Vector) { 0, 0 }, 280, "Nation name", "", &Menu_ErosionCallback));
    GUI_ContainerAdd(scene, newGame, GUI_CreateTextBox(scene, (Vector) { 0, 0 }, 280, "Map seed", "", &Menu_ErosionCallback));
    // Checkbox: AI controlled
    GUI_ContainerAdd(scene, newGame, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Start!", 0, &Menu_Back));
    GUI_ContainerAdd(scene, newGame, GUI_CreateButton(scene, (Vector) { 0, 0 }, 280, 50, "Back", 0, &Menu_Back));

    Game_PushScene(scene);
    return scene;
}