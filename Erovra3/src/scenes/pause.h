#ifndef PAUSE_H
#define PAUSE_H

#include "../engine/scene.h"

enum pauseState {
	PAUSE,
	VICTORY,
	DEFEAT,
	SURRENDER,
	RETURN_MENU,
	RETURN_MATCH
};

Scene* Pause_Init(Scene* mScene, enum pauseState s);

#endif