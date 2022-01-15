#include "../gui/gui.h"
#include "./assemblages.h"
#include <string.h>

EntityID UnitList_Create(struct scene* scene)
{
    EntityID unitListID = Scene_NewEntity(scene);

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
    Scene_Assign(scene, unitListID, GUI_COMPONENT_ID, &gui);

    Scene_Assign(scene, unitListID, UNIT_LIST_COMPONENT_ID, NULL);

    return unitListID;
}