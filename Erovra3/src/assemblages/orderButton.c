#include "../gui/gui.h"
#include "./assemblages.h"
#include <string.h>

EntityID OrderButton_Create(struct scene* scene, char* text, SDL_Texture* icon, UnitType type, GUICallback onclick)
{
    EntityID buttonID = Scene_NewEntity(scene);

    int textWidth = FC_GetWidth(font, text) + 2 * 14;

    GUIComponent gui = {
        false,
        false,
        (Vector) { 0, 0 },
        204,
        48,
        14,
        2,
        0,
        true,
        INVALID_ENTITY_INDEX,
    };
    Scene_Assign(scene, buttonID, GUI_COMPONENT_ID, &gui);

    Clickable button = {
        onclick,
        type
    };
    strncpy_s(button.text, 32, text, 32);
    Scene_Assign(scene, buttonID, GUI_CLICKABLE_COMPONENT_ID, &button);

    OrderButton orderButton = {
        icon,
        type
    };
    Scene_Assign(scene, buttonID, ORDER_BUTTON_COMPONENT_ID, &orderButton);

    return buttonID;
}