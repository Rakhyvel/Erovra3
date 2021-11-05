/*	soundManager.h
* 
*	@author	Joseph Shimel
*	@date	9/6/21
*/

#include "soundManager.h"


void Sound_Play(Mix_Chunk* sound)
{
    Mix_PlayChannel(-1, sound, 0);
}