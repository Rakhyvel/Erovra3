#include "orderButton.h"
#include "../gui/gui.h"

EntityID OrderButton_Create(struct scene* scene, char* text, TextureID icon, UnitType type, GUICallback onclick)
{
    EntityID buttonID = Scene_NewEntity(scene);

    int textWidth = Font_GetWidth(text) + 2 * 14;

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