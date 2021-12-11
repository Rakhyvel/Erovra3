#pragma once
#include "../engine/apricot.h"
#include "../engine/soundManager.h"
#include "../util/debug.h"
#include "gui.h"
#include <ctype.h>
#include <string.h>

SDL_Texture* radioUnchecked = NULL;
SDL_Texture* radioChecked = NULL;
SDL_Texture* check = NULL;

Mix_Chunk* HOVER_SOUND;
Mix_Chunk* CLICK_SOUND;

SDL_Color backgroundColor = { 57, 63, 68, 255 };
SDL_Color textColor = { 255, 255, 255, 255 };
SDL_Color borderColor = { 133, 136, 140, 255 };
SDL_Color hoverColor = { 255, 255, 255, 255 };
SDL_Color activeColor = { 60, 100, 250, 255 };
SDL_Color errorColor = { 250, 80, 80, 255 };
SDL_Color inactiveBackgroundColor = { 77, 82, 88, 64 };
SDL_Color inactiveTextColor = { 200, 200, 200, 255 };

FC_Font* font = NULL;
FC_Font* bigFont = NULL;
static int kern16[] = {
    0, 0, 4, 4, 4, 1, 7, 16, 7, 0, 0, 6, 5, 0, 6, 5,
    7, 6, 5, 4, 5, 7, 7, 4, 5, 4, 6, 6, 0, 0, 0, 0,
    8, 3, 5, 9, 7, 12, 9, 2, 4, 4, 5, 8, 2, 4, 2, 6,
    7, 5, 7, 7, 8, 7, 7, 7, 7, 7, 2, 2, 8, 8, 8, 7,
    13, 11, 9, 9, 9, 9, 8, 10, 9, 3, 7, 9, 7, 11, 9, 10,
    9, 10, 9, 9, 9, 9, 10, 13, 9, 10, 7, 4, 5, 4, 5, 9,
    3, 7, 7, 7, 7, 7, 6, 7, 7, 3, 4, 7, 3, 11, 7, 7,
    8, 7, 5, 7, 5, 7, 7, 11, 7, 7, 8, 5, 3, 5, 8, 4
};

/*	 */
void GUI_Init()
{
    GUI_COMPONENT_ID = rand();
    GUI_BUTTON_COMPONENT_ID = rand();
    GUI_CLICKABLE_COMPONENT_ID = rand();
    GUI_LABEL_ID = rand();
    GUI_ROCKER_SWITCH_COMPONENT_ID = rand();
    GUI_RADIO_BUTTONS_COMPONENT_ID = rand();
    GUI_SLIDER_COMPONENT_ID = rand();
    GUI_TEXT_BOX_COMPONENT_ID = rand();
    GUI_CHECK_BOX_COMPONENT_ID = rand();
    GUI_IMAGE_COMPONENT_ID = rand();
    GUI_PROGRESS_BAR_COMPONENT_ID = rand();
    GUI_CONTAINER_COMPONENT_ID = rand();
    GUI_CENTERED_COMPONENT_ID = rand();

    radioUnchecked = Texture_Load("res/gui/radio.png");

    radioChecked = Texture_Load("res/gui/radioFill.png");

    check = Texture_Create(200, 200);
    Texture_DrawPolygon(check, Polygon_Create("res/gui/check.gon"), hoverColor, 10);

    HOVER_SOUND = Mix_LoadWAV("res/hover.wav");
    CLICK_SOUND = Mix_LoadWAV("res/click.wav");

    if (!font) {
        font = FC_CreateFont();
        bigFont = FC_CreateFont();
        FC_LoadFont(font, Apricot_Renderer, "res/gui/Segoe UI.ttf", 16, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
        FC_LoadFont(bigFont, Apricot_Renderer, "res/gui/Segoe UI.ttf", 175, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
    }
}

/*	 */
void GUI_Register(Scene* scene)
{
    Scene_RegisterComponent(scene, GUI_COMPONENT_ID, sizeof(GUIComponent));
    Scene_RegisterComponent(scene, GUI_BUTTON_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, GUI_CLICKABLE_COMPONENT_ID, sizeof(Clickable));
    Scene_RegisterComponent(scene, GUI_LABEL_ID, sizeof(Label));
    Scene_RegisterComponent(scene, GUI_ROCKER_SWITCH_COMPONENT_ID, sizeof(RockerSwitch));
    Scene_RegisterComponent(scene, GUI_RADIO_BUTTONS_COMPONENT_ID, sizeof(RadioButtons));
    Scene_RegisterComponent(scene, GUI_SLIDER_COMPONENT_ID, sizeof(Slider));
    Scene_RegisterComponent(scene, GUI_TEXT_BOX_COMPONENT_ID, sizeof(TextBox));
    Scene_RegisterComponent(scene, GUI_CHECK_BOX_COMPONENT_ID, sizeof(CheckBox));
    Scene_RegisterComponent(scene, GUI_IMAGE_COMPONENT_ID, sizeof(Image));
    Scene_RegisterComponent(scene, GUI_PROGRESS_BAR_COMPONENT_ID, sizeof(ProgressBar));
    Scene_RegisterComponent(scene, GUI_CONTAINER_COMPONENT_ID, sizeof(Container));
    Scene_RegisterComponent(scene, GUI_CENTERED_COMPONENT_ID, 0);
}

void GUI_Destroy(Scene* scene)
{
    system(scene, id, GUI_CONTAINER_COMPONENT_ID)
    {
        Container* container = (Container*)Scene_GetComponent(scene, id, GUI_CONTAINER_COMPONENT_ID);
        Arraylist_Destroy(container->children);
    }
}

/*
	Creates a button GUI entity. You can give a pos, width and height, but these
	will be overriden if the button is added to a container*/
EntityID GUI_CreateButton(Scene* scene, Vector pos, int width, int height, char* text, int meta, GUICallback onclick)
{
    EntityID buttonID = Scene_NewEntity(scene);

    int textWidth = FC_GetWidth(font, text) + 2 * 14;

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        height,
        14,
        2,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, buttonID, GUI_COMPONENT_ID, &gui);

    Clickable clickable = {
        onclick,
        meta
    };
    strncpy_s(clickable.text, 32, text, 32);
    Scene_Assign(scene, buttonID, GUI_BUTTON_COMPONENT_ID, 0);
    Scene_Assign(scene, buttonID, GUI_CLICKABLE_COMPONENT_ID, &clickable);

    return buttonID;
}

/*
	Creates a label, given a position and some text */
EntityID GUI_CreateLabel(Scene* scene, Vector pos, char* text)
{
    EntityID labelID = Scene_NewEntity(scene);

    int textWidth = FC_GetWidth(font, text);

    GUIComponent gui = {
        false,
        false,
        pos,
        textWidth,
        16,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
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

    int textWidth = FC_GetWidth(font, text) + 44 + 12;

    GUIComponent gui = {
        false,
        false,
        pos,
        textWidth,
        20,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, rockerSwitchID, GUI_COMPONENT_ID, &gui);

    RockerSwitch rockerSwitch = {
        value,
        onchange
    };
    strncpy_s(rockerSwitch.text, 255, text, 255);
    Scene_Assign(scene, rockerSwitchID, GUI_ROCKER_SWITCH_COMPONENT_ID, &rockerSwitch);
    return rockerSwitchID;
}

EntityID GUI_CreateRadioButtons(Scene* scene, Vector pos, char* groupLabel, int defaultSelection, int nSelections, char* options, ...)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        100,
        nSelections * (32 + 12) + 16,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    RadioButtons radioButtons = {
        defaultSelection,
        nSelections
    };
    strncpy_s(radioButtons.groupLabel, 32, groupLabel, 32);

    va_list args;
    va_start(args, options);
    strncpy_s(radioButtons.options[0], 32, options, 32);
    for (int i = 1; i < nSelections; i++) {
        strncpy_s(radioButtons.options[i], 32, va_arg(args, char*), 32);
    }
    va_end(args);
    Scene_Assign(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID, &radioButtons);

    return id;
}

EntityID GUI_CreateSlider(Scene* scene, Vector pos, int width, char* label, float defaultValue, int nNotches, GUICallback onupdate)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        16 + 6 + 24 + 6,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    Slider slider = {
        onupdate,
        defaultValue,
        nNotches
    };
    strncpy_s(slider.label, 32, label, 32);
    Scene_Assign(scene, id, GUI_SLIDER_COMPONENT_ID, &slider);

    return id;
}

EntityID GUI_CreateTextBox(Scene* scene, Vector pos, int width, char* label, char* defaultText, GUICallback onupdate)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        32 + 6 + 16 + 1,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    TextBox textBox = {
        false,
        onupdate
    };
    strncpy_s(textBox.label, 32, label, 32);
    strncpy_s(textBox.text, 32, defaultText, 32);
    textBox.length = strlen(textBox.text);
    Scene_Assign(scene, id, GUI_TEXT_BOX_COMPONENT_ID, &textBox);

    return id;
}

EntityID GUI_CreateCheckBox(Scene* scene, Vector pos, char* label, bool defaultValue)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        20 + 9 + FC_GetWidth(font, label),
        20,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    CheckBox checkBox = {
        defaultValue
    };
    strncpy_s(checkBox.label, 32, label, 32);
    Scene_Assign(scene, id, GUI_CHECK_BOX_COMPONENT_ID, &checkBox);

    return id;
}

EntityID GUI_CreateImage(Scene* scene, Vector pos, int width, int height, SDL_Texture* texture)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        height,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    Image image = {
        texture,
        0
    };
    Scene_Assign(scene, id, GUI_IMAGE_COMPONENT_ID, &image);

    return id;
}

EntityID GUI_CreateProgressBar(Scene* scene, Vector pos, int width, float defaultValue)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        6,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    ProgressBar progressBar = {
        defaultValue
    };
    Scene_Assign(scene, id, GUI_PROGRESS_BAR_COMPONENT_ID, &progressBar);

    return id;
}

EntityID GUI_CreateSpacer(Scene* scene, Vector pos, int width, int height)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        height,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    return id;
}

/*
	Creates a container, which holds other GUI components. You can give it a pos,
	but will be overriden if added to another container*/
EntityID GUI_CreateContainer(Scene* scene, Vector pos, int maxWidth, int maxHeight)
{
    EntityID containerID = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        0,
        0,
        14,
        0,
        0,
        true,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, containerID, GUI_COMPONENT_ID, &gui);

    Container container;
    container.maxHeight = maxHeight;
    container.maxWidth = maxWidth;
    container.children = Arraylist_Create(1, sizeof(EntityID));
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
	Changes the labels text. Takes a format string, with some convenient 
	formatting.
	
	%d - replaces with the ascii represenation of an integer
	%f - replaces with the ascii representation of a float
	%s - inserts the text at the given character pointer into the label text */
void GUI_SetLabelText(Scene* scene, EntityID labelID, char* format, ...)
{
    Label* label = (Label*)Scene_GetComponent(scene, labelID, GUI_LABEL_ID); // Get error that Entity 2 does not have component 37, mask is 0
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, labelID, GUI_COMPONENT_ID);

    memset(label->text, 0, 32);
    int labelPos = 0;
    int formatPos;
    va_list args;
    va_start(args, format);

    for (formatPos = 0; labelPos < 31 && format[formatPos] != '\0'; formatPos++) {
        if (format[formatPos] == '%') {
            char temp[32];
            memset(temp, 0, 32);
            int offset = -1;
            if (format[formatPos + 1] == 'd') {
                int arg = va_arg(args, int);
                offset = sprintf_s(temp, 32, "%d", arg);
            } else if (format[formatPos + 1] == 'f') {
                offset = sprintf_s(temp, 32, "%.2f", va_arg(args, double));
            } else if (format[formatPos + 1] == 's') {
                offset = sprintf_s(temp, 32, "%s", va_arg(args, char*));
            }
            if (offset < 0) {
                PANIC("Error printing to a stirng in GUI#SetLabelText");
            } else {
                labelPos += offset;
            }

            // Cutom strcat THAT DOESNT CRASH!
            int i;
            for (i = 0; i < 32 && label->text[i] != '\0'; i++)
                ;
            for (int j = 0; i < 32 && temp[j] != '\0'; i++, j++) {
                label->text[i] = temp[j];
            }

            label->text[labelPos + 1] = '\0';
            formatPos++;
        } else {
            label->text[labelPos++] = format[formatPos];
        }
    }
    va_end(args);

    int oldWidth = gui->width;
    int textWidth = FC_GetWidth(font, label->text);
    if (oldWidth != textWidth) {
        gui->width = textWidth;
        GUIComponent* root = (GUIComponent*)Scene_GetComponent(scene, GUI_GetRoot(scene, labelID), GUI_COMPONENT_ID);
        GUI_UpdateLayout(scene, GUI_GetRoot(scene, labelID), root->pos.x, root->pos.y);
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
	Adds a GUI component to a container. Will update layout */
void GUI_ContainerAdd(Scene* scene, EntityID containerID, EntityID object)
{
    // Should probably check that the ID's have the right components
    Container* container = (Container*)Scene_GetComponent(scene, containerID, GUI_CONTAINER_COMPONENT_ID);
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, containerID, GUI_COMPONENT_ID);
    GUIComponent* objectGui = (GUIComponent*)Scene_GetComponent(scene, object, GUI_COMPONENT_ID);
    Arraylist_Add(&container->children, &object);
    EntityID id = *(EntityID*)Arraylist_Get(container->children, 0);
    objectGui->parent = containerID;
    GUI_UpdateLayout(scene, GUI_GetRoot(scene, containerID), gui->pos.x, gui->pos.y);
}

/*
	Changes the whether the GUI component is shown or not. If gui component is 
	container, will udpate all children. Will update layout tree. */
void GUI_SetShown(Scene* scene, EntityID id, bool shown)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    gui->shown = shown;

    if (Scene_EntityHasComponents(scene, id, GUI_CONTAINER_COMPONENT_ID)) {
        Container* container = (Container*)Scene_GetComponent(scene, id, GUI_CONTAINER_COMPONENT_ID);
        for (int i = 0; i < container->children->size; i++) {
            EntityID childID = *(EntityID*)Arraylist_Get(container->children, i);
            GUIComponent* childGUI = (GUIComponent*)Scene_GetComponent(scene, childID, GUI_COMPONENT_ID);
            childGUI->shown = shown;
            if (Scene_EntityHasComponents(scene, childID, GUI_CONTAINER_COMPONENT_ID)) {
                GUI_SetShown(scene, childID, shown);
            }
        }
    }

    GUIComponent* root = (GUIComponent*)Scene_GetComponent(scene, GUI_GetRoot(scene, id), GUI_COMPONENT_ID);
    GUI_UpdateLayout(scene, GUI_GetRoot(scene, id), root->pos.x, root->pos.y);
}

/*
	Centers a container around the center of the screen, plus an offset */
void GUI_CenterElementAt(Scene* scene, EntityID id, int x, int y)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    gui->pos.x = Apricot_Width / 2 - gui->width / 2 + x;
    gui->pos.y = Apricot_Height / 2 - gui->height / 2 + y;
    GUI_UpdateLayout(scene, GUI_GetRoot(scene, id), gui->pos.x, gui->pos.y);
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

    if (gui->parent != INVALID_ENTITY_INDEX) {
        gui->pos.x = parentX + gui->margin + gui->padding; // Should be margin really
        gui->pos.y = parentY + gui->margin + gui->padding;
    } else {
        gui->pos.x = parentX;
        gui->pos.y = parentY;
    }
    if (Scene_EntityHasComponents(scene, id, GUI_CONTAINER_COMPONENT_ID)) {
        Vector size = { gui->padding, gui->padding }; // The working size of the container. Will be returned.
        Container* container = (Container*)Scene_GetComponent(scene, id, GUI_CONTAINER_COMPONENT_ID);
        Vector placement = { gui->margin, gui->margin }; // Offset from containers top-left corner, where children will be placed.
        if (Scene_EntityHasComponents(scene, id, GUI_CENTERED_COMPONENT_ID)) {
            for (int i = 0; i < container->children->size; i++) {
                EntityID childID = *(EntityID*)Arraylist_Get(container->children, i);
                GUIComponent* childGUI = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
                Vector newSize = GUI_UpdateLayout(scene, childID, gui->pos.x, gui->pos.y);
                if (newSize.x != -1) {
                    size.x = max(size.x, newSize.x + 2 * childGUI->margin);
                    size.y += newSize.y + gui->padding + 2 * childGUI->margin;
                }
            }
            // Size is now defined
            for (int i = 0; i < container->children->size; i++) {
                EntityID childID = *(EntityID*)Arraylist_Get(container->children, i);
                GUIComponent* childGUI = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
                Vector newSize = GUI_UpdateLayout(scene, childID, gui->pos.x + placement.x, gui->pos.y + placement.y);
                GUI_UpdateLayout(scene, childID, gui->pos.x + size.x / 2 - newSize.x / 2 + childGUI->margin, gui->pos.y + placement.y);
                placement.y += newSize.y + gui->padding + 2 * childGUI->margin;
            }
        } else {
            bool overFlow = false;
            for (int i = 0; i < container->children->size; i++) {
                EntityID childID = *(EntityID*)Arraylist_Get(container->children, i);
                Vector newSize = GUI_UpdateLayout(scene, childID, gui->pos.x + placement.x, gui->pos.y + placement.y);
                if (newSize.x != -1) {
                    if (!overFlow && container->maxHeight != -1 && placement.y + newSize.y + gui->padding > container->maxHeight) {
                        placement.y = gui->margin;
                        placement.x = size.x + gui->padding;
                        size.y = max(size.y, placement.y + newSize.y + gui->padding);
                        i--; // Repeat item
                        overFlow = true;
                    } else {
                        placement.y += newSize.y + gui->padding;
                        size.y = max(size.y, placement.y + gui->padding);
                        overFlow = false;
                    }
                    size.x = max(size.x, placement.x + newSize.x);
                }
            }
        }
        size.x += 2 * gui->padding + gui->margin;
        if (container->maxWidth == -1) {
            gui->width = size.x;
        } else {
            gui->width = container->maxWidth;
        }
        if (container->maxHeight == -1) {
            gui->height = size.y;
        } else {
            gui->height = container->maxHeight;
        }
        return size;
    } else {
        return (Vector) { gui->width, gui->height };
    }
}

void GUI_SetBackgroundColor(Scene* scene, EntityID id, SDL_Color color)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    gui->backgroundColor = color;
}

void GUI_SetPadding(Scene* scene, EntityID id, int padding)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    gui->padding = padding;
}

void GUI_SetBorder(Scene* scene, EntityID id, int border)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    gui->border = border;
}

void GUI_SetMargin(Scene* scene, EntityID id, int margin)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
    gui->margin = margin;
}

// UPDATING
/*
	Updates a button. Checks to see if it's hovered, and then clicked */
static void updateClickable(Scene* scene)
{
    system(scene, id, GUI_CLICKABLE_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        Clickable* clickable = (Clickable*)Scene_GetComponent(scene, id, GUI_CLICKABLE_COMPONENT_ID);
        bool prevIsHovered = gui->isHovered;
        gui->isHovered = gui->shown && Apricot_MousePos.x > gui->pos.x && Apricot_MousePos.x < gui->pos.x + gui->width && Apricot_MousePos.y > gui->pos.y && Apricot_MousePos.y < gui->pos.y + gui->height;
        if (!prevIsHovered && gui->isHovered && gui->active) {
            Sound_Play(HOVER_SOUND);
        }
        if (gui->isHovered && Apricot_MouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        gui->border = gui->isHovered * 2;
        if (Apricot_MouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                if (clickable->onclick == NULL) {
                    PANIC("Button onclick is NULL for button %s", clickable->text);
                }
                if (gui->active) {
                    Sound_Play(CLICK_SOUND);
                }
                clickable->onclick(scene, id);
            }
            gui->clickedIn = false;
        }
    }
}

/*
	Updates a rocker switch. Checks to see if it's hovered, and then if its 
	clicked. When clicked, inverts boolean value, calls callback with new value
	passed */
static void updateRockerSwitch(Scene* scene)
{
    system(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        RockerSwitch* rockerSwitch = (RockerSwitch*)Scene_GetComponent(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID);
        bool prevIsHovered = gui->isHovered;
        gui->isHovered = gui->shown && Apricot_MousePos.x > gui->pos.x && Apricot_MousePos.x < gui->pos.x + gui->width && Apricot_MousePos.y > gui->pos.y && Apricot_MousePos.y < gui->pos.y + gui->height;
        if (!prevIsHovered && gui->isHovered && gui->active) {
            Sound_Play(HOVER_SOUND);
        }
        if (gui->isHovered && Apricot_MouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (Apricot_MouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                if (rockerSwitch->onchange == NULL) {
                    PANIC("Rocker switch onchange is NULL for rocker switch %s", rockerSwitch->text);
                }
                if (gui->active) {
                    Sound_Play(CLICK_SOUND);
                }
                rockerSwitch->value = !rockerSwitch->value;
                rockerSwitch->onchange(scene, id);
            }
            gui->clickedIn = false;
        }
    }
}

void updateRadioButtons(Scene* scene)
{
    const int buttonHeight = 32;
    const int buttonPadding = 12;
    system(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        // int buttonY = gui->pos.y + i * (buttonHeight + buttonPadding) + buttonPadding + 16 + 6;
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        RadioButtons* radioButtons = (RadioButtons*)Scene_GetComponent(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID);

        gui->isHovered = gui->shown && Apricot_MousePos.x > gui->pos.x && Apricot_MousePos.x < gui->pos.x + gui->width && Apricot_MousePos.y > gui->pos.y + 34 && Apricot_MousePos.y < gui->pos.y + gui->height;
        int oldSelected = radioButtons->selectionHovered;
        if (gui->isHovered) {
            radioButtons->selectionHovered = (Apricot_MousePos.y - gui->pos.y - 22) / 44;
        } else {
            radioButtons->selectionHovered = -1;
        }
        if (radioButtons->selectionHovered != -1 && oldSelected != radioButtons->selectionHovered) {
            Sound_Play(HOVER_SOUND);
        }
        if (gui->isHovered && Apricot_MouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (Apricot_MouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                radioButtons->selection = radioButtons->selectionHovered;
                if (gui->active) {
                    Sound_Play(CLICK_SOUND);
                }
            }
            gui->clickedIn = false;
        }
    }
}

void updateSlider(Scene* scene)
{
    const int buttonHeight = 32;
    const int buttonPadding = 12;
    system(scene, id, GUI_SLIDER_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        // int buttonY = gui->pos.y + i * (buttonHeight + buttonPadding) + buttonPadding + 16 + 6;
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Slider* slider = (Slider*)Scene_GetComponent(scene, id, GUI_SLIDER_COMPONENT_ID);

        bool prevIsHovered = gui->isHovered;
        gui->isHovered = gui->shown && Apricot_MousePos.x > gui->pos.x && Apricot_MousePos.x < gui->pos.x + gui->width && Apricot_MousePos.y > gui->pos.y + 6 + 16 && Apricot_MousePos.y < gui->pos.y + gui->height;
        if (!prevIsHovered && gui->isHovered && gui->active) {
            Sound_Play(HOVER_SOUND);
        }
        if ((gui->isHovered || gui->clickedIn) && Apricot_MouseLeftDown) {
            float val = (Apricot_MousePos.x - gui->pos.x) / gui->width;
            if (slider->nNotches == 0) { // Notchless behavior
                slider->value = max(0.0f, min(1.0f, val));
            } else {
                slider->value = max(0.0f, min(slider->nNotches -1, ((int)round(val * (slider->nNotches - 1)))));
            }
            if (!gui->clickedIn) {
                Sound_Play(HOVER_SOUND);
            }
            gui->clickedIn = true;
        }
        if (gui->clickedIn && !Apricot_MouseLeftDown) {
            if (slider->onupdate) {
                slider->onupdate(scene, id);
            }
            if (gui->clickedIn) {
                Sound_Play(HOVER_SOUND);
            }
            gui->clickedIn = false;
        }
    }
}

void updateTextBox(Scene* scene)
{
    system(scene, id, GUI_TEXT_BOX_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        TextBox* textBox = (TextBox*)Scene_GetComponent(scene, id, GUI_TEXT_BOX_COMPONENT_ID);

        bool prevIsHovered = gui->isHovered;
        gui->isHovered = gui->shown && Apricot_MousePos.x > gui->pos.x && Apricot_MousePos.x < gui->pos.x + gui->width && Apricot_MousePos.y > gui->pos.y + 6 + 16 && Apricot_MousePos.y < gui->pos.y + gui->height;
        if (!prevIsHovered && gui->isHovered && gui->active && !textBox->active) {
            Sound_Play(HOVER_SOUND);
        }
        if (gui->isHovered && Apricot_MouseLeftDown) {
            gui->clickedIn = true;
        }
        if (!gui->isHovered && Apricot_MouseLeftDown) {
            gui->clickedIn = false;
            if (textBox->active && textBox->onupdate != NULL) {
                textBox->onupdate(scene, id);
            }
            textBox->active = false;
        }
        if (gui->clickedIn && !Apricot_MouseLeftDown) {
            gui->clickedIn = false;
            textBox->active = true;
            int total = 0;
            /* Get character index */ {
                int x = Apricot_MousePos.x - gui->pos.x - 9;
                for (int i = 0; textBox->text[i] != '\0'; i++) {
                    total += kern16[textBox->text[i]] + 1;
                    if (total > x + (kern16[textBox->text[i]] + 1) / 2) {
                        total = i;
                        break;
                    }
                }
            }
            textBox->cursorPos = max(0, min(textBox->length, total));
            if (gui->active) {
                Sound_Play(CLICK_SOUND);
            }
        }
        if (textBox->active && Apricot_CharDown != '\0') {
            // Place character in textApricot_K
            if (Apricot_CharDown >= ' ' && Apricot_CharDown <= '~' && textBox->length < 31) {
                for (int i = textBox->length; i > textBox->cursorPos; i--) {
                    textBox->text[i] = textBox->text[i - 1];
                }
                textBox->text[textBox->cursorPos] = Apricot_CharDown;
                textBox->cursorPos++;
                textBox->length++;
                textBox->text[textBox->length] = '\0';
            }
            // Backspace
            else if (Apricot_CharDown == '\b' && textBox->length > 0 && textBox->cursorPos > 0) {
                textBox->cursorPos--;
                for (int i = textBox->cursorPos; i < textBox->length; i++) {
                    textBox->text[i] = textBox->text[i + 1];
                }
                textBox->length--;
                textBox->text[textBox->length] = '\0';
            }
            // Left arrow
            else if (Apricot_Keys[SDL_SCANCODE_LEFT] && textBox->cursorPos > 0) {
                textBox->cursorPos--;
            }
            // Right arrow
            else if (Apricot_Keys[SDL_SCANCODE_RIGHT] && textBox->cursorPos < textBox->length) {
                textBox->cursorPos++;
            }
        }
    }
}

static void updateCheckBox(Scene* scene)
{
    system(scene, id, GUI_CHECK_BOX_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        CheckBox* checkBox = (CheckBox*)Scene_GetComponent(scene, id, GUI_CHECK_BOX_COMPONENT_ID);
        bool prevIsHovered = gui->isHovered;
        gui->isHovered = gui->shown && Apricot_MousePos.x > gui->pos.x && Apricot_MousePos.x < gui->pos.x + gui->width && Apricot_MousePos.y > gui->pos.y && Apricot_MousePos.y < gui->pos.y + gui->height;
        if (!prevIsHovered && gui->isHovered && gui->active) {
            Sound_Play(HOVER_SOUND);
        }
        if (gui->isHovered && Apricot_MouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (Apricot_MouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                checkBox->value = !checkBox->value;
                if (gui->active) {
                    Sound_Play(CLICK_SOUND);
                }
            }
            gui->clickedIn = false;
        }
    }
}

/*
	Runs all GUI update systems */
void GUI_Update(Scene* scene)
{
    updateClickable(scene);
    updateRockerSwitch(scene);
    updateRadioButtons(scene);
    updateSlider(scene);
    updateTextBox(scene);
    updateCheckBox(scene);
}

// RENDERING
/*
	Draws a button to the screen */
static void renderButton(Scene* scene)
{
    system(scene, id, GUI_BUTTON_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Clickable* clickable = (Clickable*)Scene_GetComponent(scene, id, GUI_CLICKABLE_COMPONENT_ID);
        SDL_SetRenderDrawColor(Apricot_Renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
        SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        if (gui->isHovered) {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_RenderFillRect(Apricot_Renderer, &rect);
            SDL_SetRenderDrawColor(Apricot_Renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        }
        FC_Draw(font, Apricot_Renderer, gui->pos.x + (gui->width - FC_GetWidth(font, clickable->text)) / 2, gui->pos.y + gui->height / 2 - 12, clickable->text);
    }
}

/*
	Draws a rocker switch to the screen */
static void renderRockerSwitch(Scene* scene)
{
    system(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        RockerSwitch* rockerSwitch = (RockerSwitch*)Scene_GetComponent(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, 44, 20 };
        SDL_Rect switchRect = { gui->pos.x + 4, gui->pos.y + 4, 8, 12 };
        if (rockerSwitch->value) {
            SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
            SDL_RenderFillRect(Apricot_Renderer, &rect);
            SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
            switchRect.x += 28;
            SDL_RenderFillRect(Apricot_Renderer, &switchRect);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(Apricot_Renderer, &switchRect);
        }

        if (gui->isHovered) {
            SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
        } else if (rockerSwitch->value) {
            SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        }

        SDL_RenderDrawRect(Apricot_Renderer, &rect);
        rect.x++;
        rect.y++;
        rect.w -= 2;
        rect.h -= 2;
        SDL_RenderDrawRect(Apricot_Renderer, &rect);

        SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
        FC_Draw(font, Apricot_Renderer, gui->pos.x + 44 + 12, gui->pos.y + gui->height / 2 - 12, rockerSwitch->text);
    }
}

/*
	Draws a label */
static void renderLabel(Scene* scene)
{
    system(scene, id, GUI_LABEL_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Label* label = (Label*)Scene_GetComponent(scene, id, GUI_LABEL_ID);
        FC_Draw(font, Apricot_Renderer, gui->pos.x + (gui->width - FC_GetWidth(font, label->text)) / 2, gui->pos.y + gui->height / 2 - 12, label->text);
    }
}

/*
	Draws a set of radio buttons */
static void renderRadioButtons(Scene* scene)
{
    const int buttonHeight = 32;
    const int buttonPadding = 12;
    system(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        RadioButtons* radioButtons = (RadioButtons*)Scene_GetComponent(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID);
        FC_Draw(font, Apricot_Renderer, gui->pos.x, gui->pos.y - 4, radioButtons->groupLabel);
        for (int i = 0; i < radioButtons->nSelections; i++) {
            int buttonY = gui->pos.y + i * (buttonHeight + buttonPadding) + buttonPadding + 16 + 6;
            if (i == radioButtons->selection) {
                SDL_SetTextureColorMod(radioUnchecked, activeColor.r, activeColor.g, activeColor.b);
                SDL_SetTextureColorMod(radioChecked, hoverColor.r, hoverColor.g, hoverColor.b);
                Texture_Draw(radioUnchecked, gui->pos.x, buttonY, 20, 20, 0);
                Texture_Draw(radioChecked, gui->pos.x, buttonY, 20, 20, 0);
            } else if (i == radioButtons->selectionHovered) {
                SDL_SetTextureColorMod(radioUnchecked, hoverColor.r, hoverColor.g, hoverColor.b);
                Texture_Draw(radioUnchecked, gui->pos.x, buttonY, 20, 20, 0);
            } else {
                SDL_SetTextureColorMod(radioUnchecked, borderColor.r, borderColor.g, borderColor.b);
                Texture_Draw(radioUnchecked, gui->pos.x, buttonY, 20, 20, 0);
            }
            FC_Draw(font, Apricot_Renderer, gui->pos.x + 28, buttonY - 2, radioButtons->options[i]);
        }
    }
}

/*
	Draws a slider */
static void renderSlider(Scene* scene)
{
    system(scene, id, GUI_SLIDER_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        const int sliderOffset = 6 + 12 + 16;
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Slider* slider = (Slider*)Scene_GetComponent(scene, id, GUI_SLIDER_COMPONENT_ID);
        FC_Draw(font, Apricot_Renderer, gui->pos.x, gui->pos.y - 4, slider->label);
        const float sliderValueDenom = slider->nNotches == 0 ? 1 : (float)(slider->nNotches - 1);

        // Draw full slider track
        if (gui->isHovered) {
            SDL_SetRenderDrawColor(Apricot_Renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y + sliderOffset, gui->width, 2 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);

        // Draw filled slider track
        SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        rect = (SDL_Rect) { gui->pos.x, gui->pos.y + sliderOffset, gui->width * slider->value / sliderValueDenom, 2 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);

        // Draw notches
        float notchWidth = gui->width / (slider->nNotches - 1);
        SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        for (int i = 0; i < slider->nNotches; i++) {
            SDL_RenderDrawLine(Apricot_Renderer, gui->pos.x + i * notchWidth, gui->pos.y + 7 + 16, gui->pos.x + i * notchWidth, gui->pos.y + 7 + 16 + 5);
            SDL_RenderDrawLine(Apricot_Renderer, gui->pos.x + i * notchWidth, gui->pos.y + 7 + 16 + 18, gui->pos.x + i * notchWidth, gui->pos.y + 7 + 16 + 18 + 5);
        }

        // Draw knob
        if (gui->isHovered && !Apricot_MouseLeftDown) {
            SDL_SetRenderDrawColor(Apricot_Renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else if (gui->isHovered) {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        }
        rect = (SDL_Rect) { gui->pos.x + gui->width * slider->value / sliderValueDenom - 4, gui->pos.y + 7 + 16, 8, 24 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);

        // Draw tooltip
        if (gui->isHovered && Apricot_MouseLeftDown) {
            rect = (SDL_Rect) { gui->pos.x + gui->width * slider->value / sliderValueDenom - 14, gui->pos.y - 27, 28, 30 };
            SDL_SetRenderDrawColor(Apricot_Renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
            SDL_RenderFillRect(Apricot_Renderer, &rect);
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_RenderDrawRect(Apricot_Renderer, &rect);
            rect.x -= 1;
            rect.y -= 1;
            rect.w += 2;
            rect.h += 2;
            SDL_RenderDrawRect(Apricot_Renderer, &rect);
            SDL_SetRenderDrawColor(Apricot_Renderer, textColor.r, textColor.g, textColor.b, textColor.a);
            if (slider->nNotches == 0) {
                int textWidth = FC_GetWidth(font, "%.1f", slider->value);
                FC_Draw(font, Apricot_Renderer, rect.x + rect.w / 2 - textWidth / 2, rect.y + 4, "%.1f", slider->value);
            } else {
                int textWidth = FC_GetWidth(font, "%d", (int)slider->value);
                FC_Draw(font, Apricot_Renderer, rect.x + rect.w / 2 - textWidth / 2, rect.y + 4, "%d", (int)slider->value);
            }
        }
    }
}

/*
	Draws a text box */
static void renderTextBox(Scene* scene)
{
    system(scene, id, GUI_TEXT_BOX_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        TextBox* textBox = (TextBox*)Scene_GetComponent(scene, id, GUI_TEXT_BOX_COMPONENT_ID);
        FC_Draw(font, Apricot_Renderer, gui->pos.x, gui->pos.y - 4, textBox->label);

        // Draw border
        SDL_Rect rect = { gui->pos.x, gui->pos.y + 6 + 16 + 32, gui->width, 2 };
        if (textBox->active) {
            SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        } else if (gui->isHovered) {
            SDL_SetRenderDrawColor(Apricot_Renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        }
        SDL_RenderDrawRect(Apricot_Renderer, &rect);

        // Fill box
        rect = (SDL_Rect) { gui->pos.x, gui->pos.y + 6 + 16, gui->width, 32 };
        SDL_SetRenderDrawColor(Apricot_Renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderFillRect(Apricot_Renderer, &rect);

        // Draw text
        FC_Draw(font, Apricot_Renderer, gui->pos.x + 9, gui->pos.y + 6 + 16 + 7 - 4, textBox->text);

        // Draw cursor
        if (textBox->active && Apricot_Ticks % 60 < 30) {
            SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
            int cursorX;
            int length = textBox->cursorPos;
            char* str = textBox->text;
            /* Get Substring width */ {
                char buffer[255];
                memset(buffer, 0, 255);
                if (length >= 255) {
                    PANIC("Um lol?");
                }
                for (int i = 0; i < length; i++) {
                    buffer[i] = str[i];
                }
                cursorX = FC_GetWidth(font, buffer);
            }
            SDL_RenderDrawLine(Apricot_Renderer, gui->pos.x + 9 + cursorX, gui->pos.y + 6 + 16 + 7, gui->pos.x + 9 + cursorX, gui->pos.y + 6 + 32 + 7);
        }
    }
}

/*
	Draws a text box */
static void renderCheckBox(Scene* scene)
{
    system(scene, id, GUI_CHECK_BOX_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        CheckBox* checkBox = (CheckBox*)Scene_GetComponent(scene, id, GUI_CHECK_BOX_COMPONENT_ID);
        FC_Draw(font, Apricot_Renderer, gui->pos.x + 20 + 9, gui->pos.y - 2, checkBox->label);

        // Draw the box
        if (checkBox->value) {
            SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        } else if (gui->isHovered) {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y, 20, 20 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
        if (checkBox->value) {
            Texture_Draw(check, gui->pos.x, gui->pos.y, 20, 20, 0);
        }
    }
}

/*
	Draws a text box */
static void renderImage(Scene* scene)
{
    system(scene, id, GUI_IMAGE_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Image* image = (Image*)Scene_GetComponent(scene, id, GUI_IMAGE_COMPONENT_ID);

        Texture_Draw(image->texture, gui->pos.x, gui->pos.y, gui->width, gui->height, 0);
    }
}

/*
	Draws a text box */
static void renderProgressBar(Scene* scene)
{
    system(scene, id, GUI_PROGRESS_BAR_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        ProgressBar* progressBar = (ProgressBar*)Scene_GetComponent(scene, id, GUI_PROGRESS_BAR_COMPONENT_ID);

        SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, 6 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
        SDL_SetRenderDrawColor(Apricot_Renderer, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        rect = (SDL_Rect) { gui->pos.x, gui->pos.y, gui->width * progressBar->value, 6 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
    }
}

/*
	Draws a container to the screen */
static void renderContainer(Scene* scene)
{
    system(scene, id, GUI_CONTAINER_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        SDL_SetRenderDrawColor(Apricot_Renderer, gui->backgroundColor.r, gui->backgroundColor.g, gui->backgroundColor.b, gui->backgroundColor.a);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
    }
}

void renderBorders(Scene* scene)
{
    system(scene, id, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown || !gui->active) {
            continue;
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        rect.x -= 1;
        rect.y -= 1;
        rect.w += 2;
        rect.h += 2;
        SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
        for (int i = 0; i < gui->border; i++) {
            SDL_RenderDrawRect(Apricot_Renderer, &rect);
            rect.x -= 1;
            rect.y -= 1;
            rect.w += 2;
            rect.h += 2;
        }
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
    renderRadioButtons(scene);
    renderTextBox(scene);
    renderCheckBox(scene);
    renderImage(scene);
    renderProgressBar(scene);
    renderSlider(scene);
    renderBorders(scene);
}
