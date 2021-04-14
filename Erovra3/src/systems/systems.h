#pragma once
#include "../engine/scene.h"

void System_Motion(struct terrain* terrain, struct scene*);
void System_Target(struct terrain* terrain, struct scene*);
void System_Select(struct scene* scene);
void System_Attack(struct scene* scene);
void System_Render(struct scene*);