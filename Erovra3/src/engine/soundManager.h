#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SDL.h>

#define MAX_SOUNDS 255
#define INVALID_SOUND_ID 255

typedef Uint8 SoundID;

SoundID Sound_Register(char* filename);
void Sound_Play(SoundID sound);

#endif