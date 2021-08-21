#pragma once
#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../util/vector.h"

typedef void (*GUICallback)(struct scene*, EntityID);

void GUI_Init();
void GUI_Register(Scene* scene);
void GUI_Destroy(Scene* scene);

EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, int meta, GUICallback);
EntityID GUI_CreateLabel(Scene* scene, Vector pos, char* text);
EntityID GUI_CreateRockerSwitch(Scene* scene, Vector pos, char* text, bool value, GUICallback);
EntityID GUI_CreateRadioButtons(Scene* scene, Vector pos, char* groupLabel, int defaultSelection, int nSelections, char* options, ...);
EntityID GUI_CreateSlider(Scene* scene, Vector pos, int width, char* label, float defaultValue, GUICallback);
EntityID GUI_CreateTextBox(Scene* scene, Vector pos, int width, char* label, char* defaultText, GUICallback onupdate);
EntityID GUI_CreateCheckBox(Scene* scene, Vector pos, char* label, bool defaultValue);
EntityID GUI_CreateImage(Scene* scene, Vector pos, int width, int height, SDL_Texture* texture);
EntityID GUI_CreateProgressBar(Scene* scene, Vector pos, int width, float defaultValue);
EntityID GUI_CreateSpacer(Scene* scene, Vector pos, int width, int height);
EntityID GUI_CreateContainer(Scene* scene, Vector pos, int maxHeight);
void GUI_SetLabelText(Scene* scene, EntityID labelID, char* format, ...);
void GUI_SetRockerSwitchValue(Scene* scene, EntityID labelID, bool value);
void GUI_ContainerAdd(Scene* scene, EntityID containerID, EntityID object);
void GUI_SetShown(Scene* scene, EntityID containerID, bool shown);
void GUI_CenterElementAt(Scene* scene, EntityID id, int x, int y);
Vector GUI_UpdateLayout(Scene* scene, EntityID id, float parentX, float parentY);
void GUI_SetBackgroundColor(Scene* scene, EntityID id, SDL_Color color);
void GUI_Update(Scene* scene);
void GUI_Render(Scene* scene);

typedef struct guiComponent {
    bool isHovered;
    bool clickedIn;
    Vector pos;
    float width;
    float height;
    bool shown;
    EntityID parent;
} GUIComponent;
ComponentKey GUI_COMPONENT_ID;

typedef struct button {
    GUICallback onclick;
    int meta;
    char text[32];
} Button;
ComponentKey GUI_BUTTON_COMPONENT_ID;

typedef struct label {
    char text[255];
} Label;
ComponentKey GUI_LABEL_ID;

typedef struct rockerSwitch {
    bool value;
    GUICallback onchange;
    char text[255];
} RockerSwitch;
ComponentKey GUI_ROCKER_SWITCH_COMPONENT_ID;

typedef struct radioButtons {
    int selection;
    int nSelections; // Should be around 3-8.
    char groupLabel[32];
    char options[8][32];
    int selectionHovered;
} RadioButtons;
ComponentKey GUI_RADIO_BUTTONS_COMPONENT_ID;

typedef struct slider {
    GUICallback onupdate; // Called whenever the user changes the slider value
    float value;
    char label[32];
} Slider;
ComponentKey GUI_SLIDER_COMPONENT_ID;

typedef struct textBox {
    bool active;
    GUICallback onupdate; // Called whenever the text box is clicked out if
    char label[32];
    char text[32];
    int cursorPos;
    int length;
} TextBox;
ComponentKey GUI_TEXT_BOX_COMPONENT_ID;

typedef struct checkBox {
    bool value;
    char label[32];
} CheckBox;
ComponentKey GUI_CHECK_BOX_COMPONENT_ID;

typedef struct image {
    SDL_Texture* texture;
    double angle;
} Image;
ComponentKey GUI_IMAGE_COMPONENT_ID;

typedef struct progressBar {
    float value;
} ProgressBar;
ComponentKey GUI_PROGRESS_BAR_COMPONENT_ID;

typedef struct container {
    Arraylist* children;
    int maxHeight;
} Container;
ComponentKey GUI_CONTAINER_COMPONENT_ID;
ComponentKey GUI_CENTERED_COMPONENT_ID;