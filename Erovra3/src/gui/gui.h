#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

typedef void (*GUICallback)(struct scene*, EntityID);

void GUI_Init(Scene* scene);
EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, int meta, GUICallback);
EntityID GUI_CreateLabel(Scene* scene, Vector pos, char* text);
EntityID GUI_CreateRockerSwitch(Scene* scene, Vector pos, char* text, bool value, GUICallback);
EntityID GUI_CreateContainer(Scene* scene, Vector pos);
void GUI_SetLabelText(Scene* scene, EntityID labelID, char* format, ...);
void GUI_SetRockerSwitchValue(Scene* scene, EntityID labelID, bool value);
void GUI_ContainerAdd(Scene* scene, EntityID containerID, EntityID object);
void GUI_SetContainerShown(Scene* scene, EntityID containerID, bool shown);
Vector GUI_UpdateLayout(Scene* scene, EntityID id, float parentX, float parentY);
void GUI_Update(Scene* scene);
void GUI_Render(Scene* scene);

typedef struct guiComponent {
    Vector pos;
    int width;
    int height;
    bool shown;
    EntityID parent;
} GUIComponent;
ComponentID GUI_COMPONENT_ID;

typedef struct button {
    bool isHovered;
    bool clickedIn;
    GUICallback onclick;
    int meta;
    char text[255];
} Button;
ComponentID GUI_BUTTON_COMPONENT_ID;

typedef struct label {
    char text[255];
} Label;
ComponentID GUI_LABEL_ID;

typedef struct rockerSwitch {
    bool isHovered;
    bool clickedIn;
    bool value;
    GUICallback onchange;
    char text[255];
} RockerSwitch;
ComponentID GUI_ROCKER_SWITCH_COMPONENT_ID;

typedef struct container {
    Arraylist children;
} Container;
ComponentID GUI_CONTAINER_COMPONENT_ID;