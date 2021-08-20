#pragma once
#include "../engine/gameState.h"
#include "../util/debug.h"
#include "font.h"
#include "gui.h"
#include <ctype.h>
#include <string.h>

#define GUI_PADDING 14

SDL_Color backgroundColor = { 57, 63, 68, 255 };
SDL_Color borderColor = { 133, 136, 140, 255 };
SDL_Color hoverColor = { 255, 255, 255, 255 };
SDL_Color activeColor = { 43, 154, 243, 255 };
SDL_Color errorColor = { 205, 25, 11, 255 };

/*
	Call this before using GUI. Registers GUI ECS components, and fonts */
void GUI_Init(Scene* scene)
{
    GUI_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(GUIComponent));
    GUI_BUTTON_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Button));
    GUI_LABEL_ID = Scene_RegisterComponent(scene, sizeof(Label));
    GUI_ROCKER_SWITCH_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(RockerSwitch));
    GUI_RADIO_BUTTONS_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(RadioButtons));
    GUI_SLIDER_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Slider));
    GUI_TEXT_BOX_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(TextBox));
    GUI_CHECK_BOX_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(CheckBox));
    GUI_IMAGE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Image));
    GUI_PROGRESS_BAR_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(ProgressBar));
    GUI_CONTAINER_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Container));
    GUI_CENTERED_COMPONENT_ID = Scene_RegisterComponent(scene, 0);

    radioUnchecked = Texture_RegisterTexture("res/gui/radio.png");
    radioChecked = Texture_RegisterTexture("res/gui/radioFill.png");
    check = Texture_RegisterTexture("res/gui/check.png");
    Texture_DrawPolygon(check, Polygon_Create("res/gui/check.gon"), hoverColor, 10);

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
        false,
        false,
        pos,
        width,
        height,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, buttonID, GUI_COMPONENT_ID, &gui);

    Button button = {
        onclick,
        meta
    };
    strncpy_s(button.text, 32, text, 32);
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
        false,
        false,
        pos,
        textWidth,
        16,
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

    int textWidth = Font_GetWidth(text) + 2 * GUI_PADDING;

    GUIComponent gui = {
        false,
        false,
        pos,
        textWidth,
        50,
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

EntityID GUI_CreateSlider(Scene* scene, Vector pos, int width, char* label, float defaultValue, GUICallback onupdate)
{
    EntityID id = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        width,
        16 + 6 + 24 + 6,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    Slider slider = {
        onupdate,
        defaultValue
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
        32 + 6 + 16,
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
        20 + 9 + Font_GetWidth(label),
        20,
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
        2,
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
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, id, GUI_COMPONENT_ID, &gui);

    return id;
}

/*
	Creates a container, which holds other GUI components. You can give it a pos,
	but will be overriden if added to another container*/
EntityID GUI_CreateContainer(Scene* scene, Vector pos, int maxHeight)
{
    EntityID containerID = Scene_NewEntity(scene);

    GUIComponent gui = {
        false,
        false,
        pos,
        0,
        0,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, containerID, GUI_COMPONENT_ID, &gui);

    Container container;
    container.maxHeight = maxHeight;
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
            strcat_s(label->text, 32, temp);
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
    GUI_UpdateLayout(scene, GUI_GetRoot(scene, containerID), 0, 0);
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
        gui->pos.x = parentX + GUI_PADDING;
        gui->pos.y = parentY + GUI_PADDING;
    }
    if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, GUI_CONTAINER_COMPONENT_ID), id)) {
        Vector size = { 0, 0 }; // The working size of the container. Will be returned.
        Container* container = (Container*)Scene_GetComponent(scene, id, GUI_CONTAINER_COMPONENT_ID);
        Vector placement = { 0, 0 }; // Offset from containers top-left corner, where children will be placed.
        if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, GUI_CENTERED_COMPONENT_ID), id)) {
            for (int i = 0; i < container->children.size; i++) {
                EntityID childID = *(EntityID*)Arraylist_Get(&container->children, i);
                Vector newSize = GUI_UpdateLayout(scene, childID, 0, 0);
                if (newSize.x != -1) {
                    size.x = max(size.x, newSize.x);
                    size.y += newSize.y + GUI_PADDING;
                }
            }
            // Size is now defined
            for (int i = 0; i < container->children.size; i++) {
                EntityID childID = *(EntityID*)Arraylist_Get(&container->children, i);
                Vector newSize = GUI_UpdateLayout(scene, childID, gui->pos.x + placement.x, gui->pos.y + placement.y);
                GUI_UpdateLayout(scene, childID, gui->pos.x + size.x / 2 - newSize.x / 2, gui->pos.y + placement.y);
                placement.y += newSize.y + GUI_PADDING;
            }
        } else {
            for (int i = 0; i < container->children.size; i++) {
                EntityID childID = *(EntityID*)Arraylist_Get(&container->children, i);
                Vector newSize = GUI_UpdateLayout(scene, childID, gui->pos.x + placement.x, gui->pos.y + placement.y);
                if (newSize.x != -1) {
                    if (placement.y + newSize.y > container->maxHeight) {
                        placement.y = 0;
                        placement.x = size.x + GUI_PADDING;
                        size.y = max(size.y, placement.y + newSize.y + GUI_PADDING);
                        i--; // Repeat item
                    } else {
                        placement.y += newSize.y + GUI_PADDING;
                        size.y = max(size.y, placement.y);
                    }
                    size.x = max(size.x, placement.x + newSize.x);
                }
            }
        }
        size.x += 2 * GUI_PADDING;
        size.y += GUI_PADDING;
        gui->width = size.x;
        gui->height = size.y;
        return size;
    } else {
        return (Vector) { gui->width, gui->height };
    }
}

void GUI_SetBackgroundColor(Scene* scene, EntityID id, SDL_Color color)
{
    GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID); // FIXME: Mask is 0 bug here too
    //gui->backgroundColor = color;
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
        gui->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y && g->mouseY < gui->pos.y + gui->height;
        if (gui->isHovered && g->mouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (g->mouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                if (button->onclick == NULL) {
                    PANIC("Button onclick is NULL for button %s", button->text);
                }
                button->onclick(scene, id);
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
    const ComponentMask mask = Scene_CreateMask(2, GUI_ROCKER_SWITCH_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        RockerSwitch* rockerSwitch = (RockerSwitch*)Scene_GetComponent(scene, id, GUI_ROCKER_SWITCH_COMPONENT_ID);
        gui->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y && g->mouseY < gui->pos.y + gui->height;
        if (gui->isHovered && g->mouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (g->mouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                if (rockerSwitch->onchange == NULL) {
                    PANIC("Rocker switch onchange is NULL for rocker switch %s", rockerSwitch->text);
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
    const ComponentMask mask = Scene_CreateMask(2, GUI_RADIO_BUTTONS_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    const int buttonHeight = 32;
    const int buttonPadding = 12;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        // int buttonY = gui->pos.y + i * (buttonHeight + buttonPadding) + buttonPadding + 16 + 6;
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        RadioButtons* radioButtons = (RadioButtons*)Scene_GetComponent(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID);

        gui->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y + 34 && g->mouseY < gui->pos.y + gui->height;
        if (gui->isHovered) {
            radioButtons->selectionHovered = (g->mouseY - gui->pos.y - 22) / 44;
        } else {
            radioButtons->selectionHovered = -1;
        }
        if (gui->isHovered && g->mouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (g->mouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                radioButtons->selection = radioButtons->selectionHovered;
            }
            gui->clickedIn = false;
        }
    }
}

void updateSlider(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_SLIDER_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    const int buttonHeight = 32;
    const int buttonPadding = 12;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        // int buttonY = gui->pos.y + i * (buttonHeight + buttonPadding) + buttonPadding + 16 + 6;
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Slider* slider = (Slider*)Scene_GetComponent(scene, id, GUI_SLIDER_COMPONENT_ID);

        gui->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y + 6 + 16 && g->mouseY < gui->pos.y + gui->height;
        if ((gui->isHovered || gui->clickedIn) && g->mouseLeftDown) {
            float val = (g->mouseX - gui->pos.x) / gui->width;
            slider->value = max(0.0f, min(1.0f, val));
            gui->clickedIn = true;
        }
        if (gui->clickedIn && !g->mouseLeftDown) {
            slider->onupdate(scene, id);
            gui->clickedIn = false;
        }
    }
}

void updateTextBox(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_TEXT_BOX_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        TextBox* textBox = (TextBox*)Scene_GetComponent(scene, id, GUI_TEXT_BOX_COMPONENT_ID);

        gui->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y + 6 + 16 && g->mouseY < gui->pos.y + gui->height;
        if (gui->isHovered && g->mouseLeftDown) {
            gui->clickedIn = true;
        }
        if (!gui->isHovered && g->mouseLeftDown) {
            gui->clickedIn = false;
            if (textBox->active && textBox->onupdate != NULL) {
                textBox->onupdate(scene, id);
            }
            textBox->active = false;
        }
        if (gui->clickedIn && !g->mouseLeftDown) {
            gui->clickedIn = false;
            textBox->active = true;
            textBox->cursorPos = max(0, min(textBox->length, Font_GetCharIndex(textBox->text, g->mouseX - gui->pos.x - 9)));
        }
        if (textBox->active && g->keyDown != '\0') {
            if (g->keyDown >= ' ' && g->keyDown <= '~' && textBox->length < 31) {
                for (int i = textBox->length; i > textBox->cursorPos; i--) {
                    textBox->text[i] = textBox->text[i - 1];
                }
                textBox->text[textBox->cursorPos] = g->keyDown;
                textBox->cursorPos++;
                textBox->length++;
                textBox->text[textBox->length] = '\0';
            } else if (g->keyDown == '\b' && textBox->length > 0 && textBox->cursorPos > 0) {
                textBox->cursorPos--;
                for (int i = textBox->cursorPos; i < textBox->length; i++) {
                    textBox->text[i] = textBox->text[i + 1];
                }
                textBox->length--;
                textBox->text[textBox->length] = '\0';
            } else if (g->left && textBox->cursorPos > 0) {
                textBox->cursorPos--;
            } else if (g->right && textBox->cursorPos < textBox->length) {
                textBox->cursorPos++;
            }
        }
    }
}

static void updateCheckBox(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_CHECK_BOX_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        CheckBox* checkBox = (CheckBox*)Scene_GetComponent(scene, id, GUI_CHECK_BOX_COMPONENT_ID);
        gui->isHovered = gui->shown && g->mouseX > gui->pos.x && g->mouseX < gui->pos.x + gui->width && g->mouseY > gui->pos.y && g->mouseY < gui->pos.y + gui->height;
        if (gui->isHovered && g->mouseLeftDown) {
            gui->clickedIn = true; // mouse must have clicked in button, and then released in button to count as a click
        }
        if (g->mouseLeftUp) {
            if (gui->clickedIn && gui->isHovered) {
                checkBox->value = !checkBox->value;
            }
            gui->clickedIn = false;
        }
    }
}

/*
	Runs all GUI update systems */
void GUI_Update(Scene* scene)
{
    updateButton(scene);
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
    const ComponentMask mask = Scene_CreateMask(2, GUI_BUTTON_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Button* button = (Button*)Scene_GetComponent(scene, id, GUI_BUTTON_COMPONENT_ID);
        SDL_SetRenderDrawColor(g->rend, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(g->rend, &rect);
        SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        if (gui->isHovered) {
            SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_RenderFillRect(g->rend, &rect);
            SDL_SetRenderDrawColor(g->rend, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        }
        SDL_RenderDrawRect(g->rend, &rect);
        rect.x -= 1;
        rect.y -= 1;
        rect.w += 2;
        rect.h += 2;
        SDL_RenderDrawRect(g->rend, &rect);
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
            SDL_SetRenderDrawColor(g->rend, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else {
            SDL_SetRenderDrawColor(g->rend, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(g->rend, &rect);
        if (gui->isHovered) {
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
	Draws a set of radio buttons */
static void renderRadioButtons(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_RADIO_BUTTONS_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    const int buttonHeight = 32;
    const int buttonPadding = 12;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        RadioButtons* radioButtons = (RadioButtons*)Scene_GetComponent(scene, id, GUI_RADIO_BUTTONS_COMPONENT_ID);
        Font_DrawString(radioButtons->groupLabel, gui->pos.x, gui->pos.y);
        for (int i = 0; i < radioButtons->nSelections; i++) {
            int buttonY = gui->pos.y + i * (buttonHeight + buttonPadding) + buttonPadding + 16 + 6;
            if (i == radioButtons->selection) {
                Texture_ColorMod(radioUnchecked, activeColor);
                Texture_ColorMod(radioChecked, hoverColor);
                Texture_Draw(radioUnchecked, gui->pos.x, buttonY, 20, 20, 0);
                Texture_Draw(radioChecked, gui->pos.x, buttonY, 20, 20, 0);
            } else if (i == radioButtons->selectionHovered) {
                Texture_ColorMod(radioUnchecked, hoverColor);
                Texture_Draw(radioUnchecked, gui->pos.x, buttonY, 20, 20, 0);
            } else {
                Texture_ColorMod(radioUnchecked, borderColor);
                Texture_Draw(radioUnchecked, gui->pos.x, buttonY, 20, 20, 0);
            }
            Font_DrawString(radioButtons->options[i], gui->pos.x + 28, buttonY + 3);
        }
    }
}

/*
	Draws a slider */
static void renderSlider(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_SLIDER_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Slider* slider = (Slider*)Scene_GetComponent(scene, id, GUI_SLIDER_COMPONENT_ID);
        Font_DrawString(slider->label, gui->pos.x, gui->pos.y);

        // Draw full slider track
        if (gui->isHovered) {
            SDL_SetRenderDrawColor(g->rend, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else {
            SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y + 6 + 12 + 16, gui->width, 2 };
        SDL_RenderFillRect(g->rend, &rect);

        // Draw filled slider track
        SDL_SetRenderDrawColor(g->rend, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        rect = (SDL_Rect) { gui->pos.x, gui->pos.y + 6 + 12 + 16, gui->width * slider->value, 2 };
        SDL_RenderFillRect(g->rend, &rect);

        // Draw knob
        if (gui->isHovered && !g->mouseLeftDown) {
            SDL_SetRenderDrawColor(g->rend, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else if (gui->isHovered) {
            SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        } else {
            SDL_SetRenderDrawColor(g->rend, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        }
        rect = (SDL_Rect) { gui->pos.x + gui->width * slider->value - 4, gui->pos.y + 6 + 16, 8, 24 };
        SDL_RenderFillRect(g->rend, &rect);
    }
}

/*
	Draws a text box */
static void renderTextBox(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_TEXT_BOX_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        TextBox* textBox = (TextBox*)Scene_GetComponent(scene, id, GUI_TEXT_BOX_COMPONENT_ID);
        Font_DrawString(textBox->label, gui->pos.x, gui->pos.y);

        // Draw border
        SDL_Rect rect = { gui->pos.x, gui->pos.y + 6 + 16 + 32, gui->width, 2 };
        if (textBox->active) {
            SDL_SetRenderDrawColor(g->rend, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        } else if (gui->isHovered) {
            SDL_SetRenderDrawColor(g->rend, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        } else {
            SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        }
        SDL_RenderDrawRect(g->rend, &rect);

        // Fill box
        rect = (SDL_Rect) { gui->pos.x, gui->pos.y + 6 + 16, gui->width, 32 };
        SDL_SetRenderDrawColor(g->rend, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderFillRect(g->rend, &rect);

        // Draw text
        Font_DrawString(textBox->text, gui->pos.x + 9, gui->pos.y + 6 + 16 + 7);

        // Draw cursor
        if (textBox->active && g->ticks % 60 < 30) {
            SDL_SetRenderDrawColor(g->rend, 255, 255, 255, 255);
            int cursorX = Font_GetSubStringWidth(textBox->text, textBox->cursorPos);
            SDL_RenderDrawLine(g->rend, gui->pos.x + 9 + cursorX, gui->pos.y + 6 + 16 + 7, gui->pos.x + 9 + cursorX, gui->pos.y + 6 + 32 + 7);
        }
    }
}

/*
	Draws a text box */
static void renderCheckBox(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_CHECK_BOX_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        CheckBox* checkBox = (CheckBox*)Scene_GetComponent(scene, id, GUI_CHECK_BOX_COMPONENT_ID);
        Font_DrawString(checkBox->label, gui->pos.x + 20 + 9, gui->pos.y + 2);

        // Draw the box
        if (checkBox->value) {
            SDL_SetRenderDrawColor(g->rend, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        } else if (gui->isHovered) {
            SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        } else {
            SDL_SetRenderDrawColor(g->rend, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y, 20, 20 };
        SDL_RenderFillRect(g->rend, &rect);
        if (checkBox->value) {
            Texture_Draw(check, gui->pos.x, gui->pos.y, 20, 20, 0);
        }
    }
}

/*
	Draws a text box */
static void renderImage(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_IMAGE_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Image* image = (Image*)Scene_GetComponent(scene, id, GUI_IMAGE_COMPONENT_ID);

        SDL_Rect dest = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderCopyEx(g->rend, image->texture, NULL, &dest, image->angle, NULL, SDL_FLIP_NONE);
    }
}

/*
	Draws a text box */
static void renderProgressBar(Scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, GUI_PROGRESS_BAR_COMPONENT_ID, GUI_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        ProgressBar* progressBar = (ProgressBar*)Scene_GetComponent(scene, id, GUI_PROGRESS_BAR_COMPONENT_ID);

        SDL_SetRenderDrawColor(g->rend, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, 6 };
        SDL_RenderFillRect(g->rend, &rect);
        SDL_SetRenderDrawColor(g->rend, activeColor.r, activeColor.g, activeColor.b, activeColor.a);
        rect = (SDL_Rect) { gui->pos.x, gui->pos.y, gui->width * progressBar->value, 6 };
        SDL_RenderFillRect(g->rend, &rect);
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
        //SDL_SetRenderDrawColor(g->rend, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        //SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        //SDL_RenderDrawRect(g->rend, &rect);
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
    renderSlider(scene);
    renderTextBox(scene);
    renderCheckBox(scene);
    renderImage(scene);
    renderProgressBar(scene);
}
