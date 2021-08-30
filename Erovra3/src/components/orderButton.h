#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"
#include "./components.h"
#include "../gui/gui.h"

EntityID OrderButton_Create(struct scene* scene, char* text, TextureID icon, UnitType type, GUICallback onclick);