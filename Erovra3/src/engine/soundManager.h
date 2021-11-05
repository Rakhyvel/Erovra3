/*	soundManager.h
* 
*	Plays sounds
* 
*	@todo	Implement channel stuff, reducing the volume of certain sounds depending on distance
* 
*	@author	Joseph Shimel
*	@date	9/6/21
*/

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SDL_mixer.h>

/**
 * @brief Plays a sound
 * @param sound the sound to play
*/
void Sound_Play(Mix_Chunk* sound);

#endif