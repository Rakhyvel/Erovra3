#pragma once
#include "../util/vector.h"
#include "../engine/scene.h"

void GUI_Init(Scene* scene);
EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, void(*onclick)());
EntityID GUI_CreateContainer(Scene* scene, Vector pos);
void GUI_SetLabelText(Scene* scene, EntityID labelID, char* text);
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
    void (*onclick)(struct scene*);
    char text[255];
} Button;
ComponentID GUI_BUTTON_COMPONENT_ID;

typedef struct label {
    char text[255];
} Label;
ComponentID GUI_LABEL_ID;

typedef struct container {
    Arraylist children;
} Container;
ComponentID GUI_CONTAINER_COMPONENT_ID;