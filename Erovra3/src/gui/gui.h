#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"
#include "../engine/textureManager.h"

TextureID radioUnchecked;
TextureID radioChecked;

typedef void (*GUICallback)(struct scene*, EntityID);

void GUI_Init(Scene* scene);
EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, int meta, GUICallback);
EntityID GUI_CreateLabel(Scene* scene, Vector pos, char* text);
EntityID GUI_CreateRockerSwitch(Scene* scene, Vector pos, char* text, bool value, GUICallback);
EntityID GUI_CreateRadioButtons(Scene* scene, Vector pos, char* groupLabel, int defaultSelection, int nSelections, char* options, ...);
EntityID GUI_CreateSlider(Scene* scene, Vector pos, int width, char* label, float defaultValue, GUICallback);
EntityID GUI_CreateContainer(Scene* scene, Vector pos);
void GUI_SetLabelText(Scene* scene, EntityID labelID, char* format, ...);
void GUI_SetRockerSwitchValue(Scene* scene, EntityID labelID, bool value);
void GUI_ContainerAdd(Scene* scene, EntityID containerID, EntityID object);
void GUI_SetContainerShown(Scene* scene, EntityID containerID, bool shown);
Vector GUI_UpdateLayout(Scene* scene, EntityID id, float parentX, float parentY);
void GUI_SetBackgroundColor(Scene* scene, EntityID id, SDL_Color color);
void GUI_Update(Scene* scene);
void GUI_Render(Scene* scene);

typedef struct guiComponent {
    bool isHovered;
    bool clickedIn;
    Vector pos;
    int width;
    int height;
    bool shown;
    EntityID parent;
    SDL_Color backgroundColor;
    SDL_Color hoverColor;
    SDL_Color activeColor;
} GUIComponent;
ComponentID GUI_COMPONENT_ID;

typedef struct button {
    GUICallback onclick;
    int meta;
    char text[32];
} Button;
ComponentID GUI_BUTTON_COMPONENT_ID;

typedef struct label {
    char text[32];
} Label;
ComponentID GUI_LABEL_ID;

typedef struct rockerSwitch {
    bool value;
    GUICallback onchange;
    char text[255];
} RockerSwitch;
ComponentID GUI_ROCKER_SWITCH_COMPONENT_ID;

typedef struct radioButtons {
    int selection;
    int nSelections; // Should be around 3-8.
    char groupLabel[32];
    char options[8][32];
} RadioButtons;
ComponentID GUI_RADIO_BUTTONS_COMPONENT_ID;

typedef struct slider {
    GUICallback onupdate; // Called whenever the user changes the slider value
    float value;
    char label[32];
} Slider;
ComponentID GUI_SLIDER_COMPONENT_ID;

typedef struct container {
    Arraylist children;
} Container;
ComponentID GUI_CONTAINER_COMPONENT_ID;