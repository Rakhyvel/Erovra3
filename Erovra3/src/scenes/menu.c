#pragma once
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "match.h"

/*
	Starts a new game */
void Menu_NewGame()
{
    Match_Init();
}

/*
	Called every tick */
void Menu_Update(Scene* scene)
{
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

    EntityID container = GUI_CreateContainer(scene, (Vector) { 100, 100 });
    GUI_ContainerAdd(scene, container, GUI_CreateButton(scene, (Vector) { 100, 100 }, 150, 50, "Start New Game", &Menu_NewGame));
    GUI_ContainerAdd(scene, container, GUI_CreateButton(scene, (Vector) { 100, 100 }, 150, 50, "Report a Bug", NULL));
    GUI_ContainerAdd(scene, container, GUI_CreateButton(scene, (Vector) { 100, 100 }, 150, 50, "Exit", NULL));

    Game_PushScene(scene);
    return scene;
}