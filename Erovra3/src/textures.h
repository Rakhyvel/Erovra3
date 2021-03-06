#pragma once
#include "engine/soundManager.h"
#include <SDL.h>

SDL_Texture* CITY_TEXTURE_ID;
SDL_Texture* CITY_OUTLINE_TEXTURE_ID;
SDL_Texture* CITY_SHADOW_TEXTURE_ID;
SDL_Texture* CAPITAL_TEXTURE_ID;
SDL_Texture* CAPITAL_OUTLINE_TEXTURE_ID;
SDL_Texture* CAPITAL_SHADOW_TEXTURE_ID;
SDL_Texture* TIMBERLAND_TEXTURE_ID;
SDL_Texture* TIMBERLAND_OUTLINE_TEXTURE_ID;
SDL_Texture* TIMBERLAND_SHADOW_TEXTURE_ID;
SDL_Texture* MINE_TEXTURE_ID;
SDL_Texture* MINE_OUTLINE_TEXTURE_ID;
SDL_Texture* MINE_SHADOW_TEXTURE_ID;
SDL_Texture* POWERPLANT_TEXTURE_ID;
SDL_Texture* POWERPLANT_OUTLINE_TEXTURE_ID;
SDL_Texture* POWERPLANT_SHADOW_TEXTURE_ID;
SDL_Texture* FOUNDRY_TEXTURE_ID;
SDL_Texture* FOUNDRY_OUTLINE_TEXTURE_ID;
SDL_Texture* FOUNDRY_SHADOW_TEXTURE_ID;
SDL_Texture* FACTORY_TEXTURE_ID;
SDL_Texture* FACTORY_OUTLINE_TEXTURE_ID;
SDL_Texture* FACTORY_SHADOW_TEXTURE_ID;
SDL_Texture* PORT_TEXTURE_ID;
SDL_Texture* PORT_OUTLINE_TEXTURE_ID;
SDL_Texture* PORT_SHADOW_TEXTURE_ID;
SDL_Texture* AIRFIELD_TEXTURE_ID;
SDL_Texture* AIRFIELD_OUTLINE_TEXTURE_ID;
SDL_Texture* AIRFIELD_SHADOW_TEXTURE_ID;
SDL_Texture* FARM_TEXTURE_ID;
SDL_Texture* FARM_OUTLINE_TEXTURE_ID;
SDL_Texture* FARM_SHADOW_TEXTURE_ID;
SDL_Texture* ACADEMY_TEXTURE_ID;
SDL_Texture* ACADEMY_OUTLINE_TEXTURE_ID;
SDL_Texture* ACADEMY_SHADOW_TEXTURE_ID;

SDL_Texture* WALL_TEXTURE_ID;
SDL_Texture* WALL_OUTLINE_TEXTURE_ID;
SDL_Texture* WALL_SHADOW_TEXTURE_ID;

SDL_Texture* INFANTRY_TEXTURE_ID;
SDL_Texture* CAVALRY_TEXTURE_ID;
SDL_Texture* ARTILLERY_TEXTURE_ID;
SDL_Texture* ENGINEER_TEXTURE_ID;
SDL_Texture* GROUND_OUTLINE_TEXTURE_ID;
SDL_Texture* GROUND_SHADOW_TEXTURE_ID;

SDL_Texture* LANDING_CRAFT_TEXTURE_ID;
SDL_Texture* DESTROYER_TEXTURE_ID;
SDL_Texture* CRUISER_TEXTURE_ID;
SDL_Texture* BATTLESHIP_TEXTURE_ID;
SDL_Texture* AIRCRAFT_CARRIER_TEXTURE_ID;
SDL_Texture* SHIP_OUTLINE_TEXTURE_ID;
SDL_Texture* SHIP_SHADOW_TEXTURE_ID;

SDL_Texture* FIGHTER_TEXTURE_ID;
SDL_Texture* FIGHTER_OUTLINE_TEXTURE_ID;
SDL_Texture* FIGHTER_SHADOW_TEXTURE_ID;
SDL_Texture* ATTACKER_TEXTURE_ID;
SDL_Texture* ATTACKER_OUTLINE_TEXTURE_ID;
SDL_Texture* ATTACKER_SHADOW_TEXTURE_ID;
SDL_Texture* BOMBER_TEXTURE_ID;
SDL_Texture* BOMBER_OUTLINE_TEXTURE_ID;
SDL_Texture* BOMBER_SHADOW_TEXTURE_ID;

SDL_Texture* BULLET_TEXTURE_ID;
SDL_Texture* BULLET_SHADOW_TEXTURE_ID;
SDL_Texture* SHELL_TEXTURE_ID;
SDL_Texture* SHELL_SHADOW_TEXTURE_ID;
SDL_Texture* BOMB_TEXTURE_ID;
SDL_Texture* BOMB_SHADOW_TEXTURE_ID;
SDL_Texture* COIN_TEXTURE_ID;
SDL_Texture* COIN_SHADOW_TEXTURE_ID;
SDL_Texture* ORE_TEXTURE_ID;
SDL_Texture* ORE_SHADOW_TEXTURE_ID;
SDL_Texture* TIMBER_TEXTURE_ID;
SDL_Texture* TIMBER_SHADOW_TEXTURE_ID;
SDL_Texture* COAL_TEXTURE_ID;
SDL_Texture* COAL_SHADOW_TEXTURE_ID;
SDL_Texture* METAL_TEXTURE_ID;
SDL_Texture* METAL_SHADOW_TEXTURE_ID;
SDL_Texture* POWER_TEXTURE_ID;
SDL_Texture* POWER_SHADOW_TEXTURE_ID;
SDL_Texture* FOOD_TEXTURE_ID;

SDL_Texture* ARROW_TEXTURE_ID;
SDL_Texture* ARROW_SHADOW_TEXTURE_ID;

SDL_Texture* NO_POWER_WARNING_TEXTURE_ID;
SDL_Texture* NO_COAL_WARNING_TEXTURE_ID;

SDL_Texture* TIMBER_INDICATOR_TEXTURE_ID;
SDL_Texture* ORE_INDICATOR_TEXTURE_ID;
SDL_Texture* COAL_INDICATOR_TEXTURE_ID;

void Textures_Init();
void Textures_Draw();