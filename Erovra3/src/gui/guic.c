#pragma once
#include "../engine/gameState.h"
#include "../util/debug.h"
#include "font.h"
#include "gui.h"
#include <string.h>

#define GUI_PADDING 4

/*
	Call this before using GUI. Registers GUI ECS components, and fonts */
void GUI_Init(Scene* scene)
{
    GUI_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(GUIComponent));
    GUI_BUTTON_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Button));
    GUI_LABEL_ID = Scene_RegisterComponent(scene, sizeof(Label));
    GUI_ROCKER_SWITCH_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(RockerSwitch));
    GUI_CONTAINER_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Container));
    Font_Init();
}

/*
	Creates a button GUI entity. You can give a pos, width and height, but these
	will be overriden if the button is added to a container*/
EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, int meta, GUICallback onclick)
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
        false,
        onclick,
		meta
    };
    strncpy_s(button.text, 255, text, 255);
    Scene_Assign(scene, buttonID, GUI_BUTTON_COMPONENT_ID, &button);

    return buttonID;
}

/*
	Creates a label, given a position and some text */
EntityID GUI_CreateLabel(Scene* scene, Vector pos, char* text)
{
    EntityID labelID = Scene_NewEntity(scene);

    int textWidth = Font_GetWidth(text) + 2 * GUI_PADDING;

    GUIComponent gui = {
        pos,
        textWidth,
        16,
        true,
        INVALID_ENTITY_INDEX
    };
    Scene_Assign(scene, labelID, GUI_COMPONENT_ID, &gui);
    Label label;
    strncpy_s(label.text, 255, text, 255);
    Scene_Assign(scene, labelID, GUI_LABEL_ID, &label);
    return labelID;
}

/*
	Creates a rocker switch entity, given a value and callback function */
EntityID GUI_CreateRockerSwitch(Scene* scene, Vector pos, char* text, bool value, GUICallback onchange)
{
    EntityID rockerSwitchID = Scene_NewEntity(scene);

    int textWidth = Font_GetWidth(text) + 2 * GUI_PADDING;

    GUIComponent gui = {
        pos,
        textWidth,
        50,
        true,
        INVALID_ENTITY_INDEX
    };
    Scene_Assign(scene, rockerSwitchID, GUI_COMPONENT_ID, &gui);

    RockerSwitch rockerSwitch = {
        false,
        false,
        value,
        onchange
    };
    strncpy_s(rockerSwitch.text, 255, text, 255);
    Scene_Assign(scene, rockerSwitchID, GUI_ROCKER_SWITCH_COMPONENT_ID, &rockerSwitch);
    printf("0x%X\n", rockerSwitchID);
    return rockerSwitchID;
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
	Changes the labels text. Takes a format string, with some convenient 
	formatting.
	
	%d - replaces with the ascii represenation of an integer
	%f - replaces with the ascii representation of a float
	%s - inserts the text at the given character pointer into the label text */
void GUI_SetLabelText(Scene* scene, EntityID labelID, char* format, ...)
{
    Label* label = (Label*)Scene_GetComponent(scene, labelID, GUI_LABEL_ID); // Get error that Entity 2 does not have component 37, mask is 0
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, labelID, GUI_COMPONENT_ID);

    memset(label->text, 0, 255);
    int labelPos = 0;
    int formatPos;
    va_list args;
    va_start(args, format);

    for (formatPos = 0; labelPos < 254 && format[formatPos] != '\0'; formatPos++) {
        if (format[formatPos] == '%') {
            char temp[255];
            memset(temp, 0, 255);
            int offset = -1;
            if (format[formatPos + 1] == 'd') {
                int arg = va_arg(args, int);
                offset = sprintf_s(temp, 17, "%d", arg);
            } else if (format[formatPos + 1] == 'f') {
                offset = sprintf_s(temp, 17, "%.2f", va_arg(args, double));
            } else if (format[formatPos + 1] == 's') {
                offset = sprintf_s(temp, 17, "%s", va_arg(args, char*));
            }
            if (offset < 0) {
                PANIC("Error printing to a stirng in GUI#SetLabelText");
            } else {
                labelPos += offset;
            }
            strcat_s(label->text, 255, temp);
            label->text[labelPos + 1] = '\0';
            formatPos++;
        } else {
            label->text[labelPos++] = format[formatPos];
        }
    }
    va_end(args);

    int oldWidth = gui->width;
    int textWidth = Font_GetWidth(label->text) + 2 * GUI_PADDING;
    if (oldWidth != textWidth) {
        gui->width = textWidth;
        GUI_UpdateLayout(scene, GUI_GetRoot(scene, labelID), 50, 50);
    }
}

/*
	 Sets the value of a rocker switch GUI entity */
void GUI_SetRockerSwitchValue(Scene* scene, EntityID id, bool value)
{
    RockerSwitch* rockerSwitch = (RockerSwitch*)Scene_GetComponent(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID);
    rockerSwitch->value = value;
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
    if (containerID == INVALID_ENTITY_INDEX) {
        PANIC("Invalid entity id passed to setContainerShown");
    }
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
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID); // FIXME: Mask is 0 bug here too
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
        button->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y && g->mouseY < gui->pos.y + gui->height;
        if (button->isHovered && g->mouseLeftDown) {
            button->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (g->mouseLeftUp) {
            if (button->clickedIn && button->isHovered) {
                if (button->onclick == NULL) {
                    PANIC("Button onclick is NULL for button %s", button->text);
                }
                button->onclick(scene, id);
            }
            button->clickedIn = false;
        }
    }
}

/*
	Updates a rocker switch. Checks to see if it's hovered, and then if its 
	clicked. When clicked, inverts boolean value, calls callback with new value
	passed */
static void updateRockerSwitch(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_ROCKER_SWITCH_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        RockerSwitch* rockerSwitch = (Button*)Scene_GetComponent(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID);
        rockerSwitch->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y && g->mouseY < gui->pos.y + gui->height;
        if (rockerSwitch->isHovered && g->mouseLeftDown) {
            rockerSwitch->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (g->mouseLeftUp) {
            if (rockerSwitch->clickedIn && rockerSwitch->isHovered) {
                if (rockerSwitch->onchange == NULL) {
                    PANIC("Rocker switch onchange is NULL for rocker switch %s", rockerSwitch->text);
                }
                rockerSwitch->value = !rockerSwitch->value;
                rockerSwitch->onchange(scene, id);
            }
            rockerSwitch->clickedIn = false;
        }
    }
}

/*
	Runs all GUI update systems */
void GUI_Update(Scene* scene)
{
    updateButton(scene);
    updateRockerSwitch(scene);
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
	Draws a rocker switch to the screen */
static void renderRockerSwitch(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_ROCKER_SWITCH_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        RockerSwitch* rockerSwitch = (RockerSwitch*)Scene_GetComponent(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID);
        if (rockerSwitch->value) {
            SDL_SetRenderDrawColor(g->rend, 60, 100, 250, 180);
        } else {
            SDL_SetRenderDrawColor(g->rend, 40, 40, 40, 180);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(g->rend, &rect);
        if (rockerSwitch->isHovered) {
            SDL_SetRenderDrawColor(g->rend, 128, 128, 128, 180);
            SDL_RenderFillRect(g->rend, &rect);
        }
        Font_DrawString(rockerSwitch->text, gui->pos.x + (gui->width - Font_GetWidth(rockerSwitch->text)) / 2, gui->pos.y + gui->height / 2 - 8);
    }
}
/*
	Draws a label */
static void renderLabel(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_LABEL_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Label* label = (Label*)Scene_GetComponent(scene, id, GUI_LABEL_ID);
        Font_DrawString(label->text, gui->pos.x + (gui->width - Font_GetWidth(label->text)) / 2, gui->pos.y + gui->height / 2 - 8);
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
    renderLabel(scene);
    renderRockerSwitch(scene);
}
