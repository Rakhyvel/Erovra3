#pragma once
#include "soundManager.h"
#include "../util/debug.h"
#include <SDL_mixer.h>

static Mix_Chunk* sounds[MAX_SOUNDS];
static int numSounds = 0;

SoundID Sound_Register(char* filename)
{
    if (numSounds >= MAX_SOUNDS) {
        PANIC("Texture overflow");
        return INVALID_SOUND_ID;
    } else {
        sounds[numSounds] = Mix_LoadWAV(filename);
        return numSounds++;
    }
}

void Sound_Play(SoundID soundID)
{
    Mix_PlayChannel(-1, sounds[soundID], 0);
}