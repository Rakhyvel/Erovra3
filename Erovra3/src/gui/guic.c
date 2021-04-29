#pragma once
#include "../util/debug.h"
#include "../engine/gameState.h"
#include "font.h"
#include "gui.h"
#include <string.h>

#define GUI_PADDING 4

/*
	Call this before using GUI. Registers GUI ECS components, and fonts */
void GUI_Init(Scene* scene)
{
    GUI_BUTTON_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Button));
    GUI_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(GUIComponent));
    GUI_CONTAINER_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Container));
    Font_Init();
}

/*
	Creates a button GUI entity. You can give a pos, width and height, but these
	will be overriden if the button is added to a container*/
EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, void (*onclick)())
{
    EntityID buttonID = Scene_NewEntity(scene);

    int textWidth = Font_GetWidth(text) + 2 * GUI_PADDING;

    GUIComponent gui = {
        pos,
        textWidth,
        height,
        true,
        INVALID_ENTITY_INDEX
    };
    Scene_Assign(scene, buttonID, GUI_COMPONENT_ID, &gui);

    Button button = {
        false,
        onclick
    };
    strncpy_s(button.text, 255, text, 255);
    Scene_Assign(scene, buttonID, GUI_BUTTON_COMPONENT_ID, &button);

    return buttonID;
}

/*
	Creates a container, which holds other GUI components. You can give it a pos,
	but will be overriden if added to another container*/
EntityID GUI_CreateContainer(Scene* scene, Vector pos)
{
    EntityID containerID = Scene_NewEntity(scene);

    GUIComponent gui = {
        pos,
        500,
        500,
        true,
        INVALID_ENTITY_INDEX
    };
    Scene_Assign(scene, containerID, GUI_COMPONENT_ID, &gui);

    Container container;
    (container.children) = *Arraylist_Create(1, sizeof(EntityID));
    Scene_Assign(scene, containerID, GUI_CONTAINER_COMPONENT_ID, &container);
    return containerID;
}

/*
	Returns the root of a layout tree */
EntityID GUI_GetRoot(Scene* scene, EntityID id)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    if (gui->parent == INVALID_ENTITY_INDEX) {
        return id;
    } else {
        return GUI_GetRoot(scene, gui->parent);
    }
}

/*
	Adds a GUI component to a container. Will update layout */
void GUI_ContainerAdd(Scene* scene, EntityID containerID, EntityID object)
{
    // Should probably check that the ID's have the right components
    Container* container = (Container*)Scene_GetComponent(scene, containerID, GUI_CONTAINER_COMPONENT_ID);
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, containerID, GUI_COMPONENT_ID);
    GUIComponent* objectGui = (GUIComponent*)Scene_GetComponent(scene, object, GUI_COMPONENT_ID);
    Arraylist_Add(&container->children, &object);
    EntityID id = *(EntityID*)Arraylist_Get(&container->children, 0);
    objectGui->parent = containerID;
    GUI_UpdateLayout(scene, GUI_GetRoot(scene, containerID), 50, 50);
}

/*
	Changes the whether the GUI component is shown or not. If gui component is 
	container, will udpate all children. Will update layout tree. */
void GUI_SetContainerShown(Scene* scene, EntityID containerID, bool shown)
{
    Container* container = (Container*)Scene_GetComponent(scene, containerID, GUI_CONTAINER_COMPONENT_ID);
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, containerID, GUI_COMPONENT_ID);
    gui->shown = shown;
    for (int i = 0; i < container->children.size; i++) {
        EntityID childID = *(EntityID*)Arraylist_Get(&container->children, i);
        GUIComponent* childGUI = (GUIComponent*)Scene_GetComponent(scene, childID, GUI_COMPONENT_ID);
        childGUI->shown = shown;
        if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, GUI_CONTAINER_COMPONENT_ID), childID)) {
            GUI_SetContainerShown(scene, childID, shown);
        }
    }
    GUI_UpdateLayout(scene, GUI_GetRoot(scene, containerID), 50, 50);
}

/*
	Updates the layout tree based on the sizes of children and the position of 
	parents. Returns height and width of component */
Vector GUI_UpdateLayout(Scene* scene, EntityID id, float parentX, float parentY)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    if (!gui->shown) {
        return (Vector) { -1, -1 };
    }

    gui->pos.x = parentX + GUI_PADDING;
    gui->pos.y = parentY + GUI_PADDING;
    if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, GUI_CONTAINER_COMPONENT_ID), id)) {
        Container* container = (Container*)Scene_GetComponent(scene, id, GUI_CONTAINER_COMPONENT_ID);
        Vector retval = { 0, 0 };
        for (int i = 0; i < container->children.size; i++) {
            EntityID childID = *(EntityID*)Arraylist_Get(&container->children, i);
            Vector newSize = GUI_UpdateLayout(scene, childID, gui->pos.x, gui->pos.y + retval.y);
            if (newSize.x != -1) {
                retval.x = max(retval.x, newSize.x);
                retval.y += newSize.y + GUI_PADDING;
            }
        }
        retval.x += 2 * GUI_PADDING;
        retval.y += GUI_PADDING;
        gui->width = retval.x;
        gui->height = retval.y;
        return retval;
    } else {
        return (Vector) { gui->width, gui->height };
    }
}

// UPDATING
/*
	Updates a button. Checks to see if it's hovered, and then clicked */
static void updateButton(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_BUTTON_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        Button* button = (Button*)Scene_GetComponent(scene, id, GUI_BUTTON_COMPONENT_ID);
        button->isHovered = g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y && g->mouseY < gui->pos.y + gui->height;
        if (button->isHovered && g->mouseLeftUp) {
			if (button->onclick == NULL) {
                PANIC("Button onclick is NULL");
			}
            button->onclick();
        }
    }
}

/*
	Runs all GUI update systems */
void GUI_Update(Scene* scene)
{
    updateButton(scene);
}

// RENDERING
/*
	Draws a button to the screen */
static void renderButton(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_BUTTON_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Button* button = (Button*)Scene_GetComponent(scene, id, GUI_BUTTON_COMPONENT_ID);
        SDL_SetRenderDrawColor(g->rend, 40, 40, 40, 180);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(g->rend, &rect);
        if (button->isHovered) {
            SDL_SetRenderDrawColor(g->rend, 128, 128, 128, 180);
            SDL_RenderFillRect(g->rend, &rect);
        }
        Font_DrawString(button->text, gui->pos.x + (gui->width - Font_GetWidth(button->text)) / 2, gui->pos.y + gui->height / 2 - 8);
    }
}

/*
	Draws a container to the screen */
static void renderContainer(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_CONTAINER_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        SDL_SetRenderDrawColor(g->rend, 40, 40, 40, 180);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(g->rend, &rect);
    }
}

/*
	Calls all GUI rendering systems */
void GUI_Render(Scene* scene)
{
    renderContainer(scene);
    renderButton(scene);
}
