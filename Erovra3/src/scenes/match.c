#pragma once
#include "match.h"
#include "../assemblages/assemblages.h"
#include "../engine/apricot.h"
#include "../engine/textureManager.h"
#include "../gui/gui.h"
#include "../main.h"
#include "../scenes/pause.h"
#include "../terrain.h"
#include "../textures.h"
#include "../util/debug.h"
#include "../util/misc.h"
#include "../util/noise.h"
#include "ai.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

// Useful structs
Terrain* terrain;
Lexicon* lexicon;

// Nation container
EntityID container;

// Nation labels
EntityID goldLabel;
EntityID oreLabel;
EntityID populationLabel;

// Unit GUI elements
EntityID focusedGUIContainer;
EntityID unitNameLabel;
EntityID unitHealthBar;

EntityID boardButtonID;
EntityID cancelBoardButtonID;

// Factory GUI elements
EntityID orderLabel;
EntityID timeLabel;
EntityID autoReOrderRockerSwitch;

Arraylist* /*<Message>*/ messages;

// Used by hover, select, and drawBoxSelect for box select
Vector boxTL = { -1, -1 };
Vector boxBR = { -1, -1 };
bool selectBox = false;

// Focus GUI flags
bool guiChange = false;
bool escFocus = false;
bool instantDefocus = false; // Set to true when a focused unit dies
static EntityID currShownEntity = INVALID_ENTITY_INDEX; // The entity who has it's focused GUI shown currently. Not necesarily the focused unit currently (fade up/fade down)

// Boarding flags
bool boardMode = false;
EntityID boardUnitID = INVALID_ENTITY_INDEX;

const int ticksPerLabor = 240; // 400 = standard; 240 = unit/min

Arraylist* /*<Nation>*/ nations;

static SDL_Texture* miniMapTexture = NULL;
const int miniMapSize = 256.0f;
static bool doFogOfWar = true;

// UTILITY FUNCTIONS
void Match_AddMessage(SDL_Color color, char* text, ...)
{
    struct message message;
    EXTRACT_VARARGS(message.text, text);
    message.fade = 0;
    message.color = color;
    Arraylist_Add(&messages, &message);
}

void Match_GetOrdinalSuffix(int divisionNumber, char* buffer)
{
    char* suffixes[] = { "th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th" };
    switch (divisionNumber % 100) {
    case 11:
    case 12:
    case 13:
        strcat_s(buffer, 3, suffixes[0]);
    default:
        strcat_s(buffer, 3, suffixes[divisionNumber % 10]);
    }
}

SDL_Texture* Match_LookupResourceTypeIcon(ResourceType type)
{
    switch (type) {
    case ResourceType_COIN:
        return COIN_TEXTURE_ID;
    case ResourceType_TIMBER:
        return TIMBER_TEXTURE_ID;
    case ResourceType_METAL:
        return METAL_TEXTURE_ID;
    }
    PANIC("Not a resource type: %d", type);
    return -1;
}

void Match_CopyUnitName(UnitType type, char* buffer)
{
    switch (type) {
    case UnitType_INFANTRY:
        strncat_s(buffer, 32, "Infantry Legion", 32);
        break;
    case UnitType_CAVALRY:
        strncat_s(buffer, 32, "Cavalry Troop", 32);
        break;
    case UnitType_ARTILLERY:
        strncat_s(buffer, 32, "Artillery Battery", 32);
        break;
    case UnitType_ENGINEER:
        strncat_s(buffer, 32, "Engineer Corps", 32);
        break;
    case UnitType_TIMBERLAND:
        strncat_s(buffer, 32, "Timberland", 32);
        break;
    case UnitType_CITY:
        strncat_s(buffer, 32, "City", 32);
        break;
    case UnitType_MINE:
        strncat_s(buffer, 32, "Mine", 32);
        break;
    case UnitType_POWERPLANT:
        strncat_s(buffer, 32, "Power Plant", 32);
        break;
    case UnitType_FOUNDRY:
        strncat_s(buffer, 32, "Foundry", 32);
        break;
    case UnitType_FACTORY:
        strncat_s(buffer, 32, "Factory", 32);
        break;
    case UnitType_PORT:
        strncat_s(buffer, 32, "Port", 32);
        break;
    case UnitType_AIRFIELD:
        strncat_s(buffer, 32, "Airfield", 32);
        break;
    case UnitType_FARM:
        strncat_s(buffer, 32, "Farm", 32);
        break;
    case UnitType_ACADEMY:
        strncat_s(buffer, 32, "Academy", 32);
        break;
    case UnitType_WALL:
        strncat_s(buffer, 32, "Wall", 32);
        break;
    case UnitType_DESTROYER:
        strncat_s(buffer, 32, "Destroyer Fleet", 32);
        break;
    case UnitType_CRUISER:
        strncat_s(buffer, 32, "Cruiser Fleet", 32);
        break;
    case UnitType_BATTLESHIP:
        strncat_s(buffer, 32, "Battleship Fleet", 32);
        break;
    case UnitType_FIGHTER:
        strncat_s(buffer, 32, "Fighter Wing", 32);
        break;
    case UnitType_ATTACKER:
        strncat_s(buffer, 32, "Attacker Wing", 32);
        break;
    case UnitType_BOMBER:
        strncat_s(buffer, 32, "Bomber Wing", 32);
        break;
    default:
        strncat_s(buffer, 32, "None", 32);
        break;
    }
}

bool Match_Collision(Scene* scene, EntityID id, Vector pos)
{
    Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
    float dx = sprite->pos.x - pos.x;
    float dy = pos.y - sprite->pos.y + sprite->z;
    if (sprite->z == -1) {
        dy += 32;
    }

    float sin = sinf(sprite->angle);
    float cos = cosf(sprite->angle);

    bool checkLR = fabs(sin * dx + cos * dy) <= sprite->height / 2;
    bool checkTB = fabs(cos * dx - sin * dy) <= sprite->width / 2;

    return checkLR && checkTB;
}

bool Match_CityHasType(Scene* scene, City* city, UnitType type)
{
    for (int i = 0; i < 4; i++) {
        if (city->expansions[i] != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, city->expansions[i], UNIT_COMPONENT_ID)) {
            Unit* unit = (Unit*)Scene_GetComponent(scene, city->expansions[i], UNIT_COMPONENT_ID);
            if (unit->type == type) {
                return true;
            }
        }
    }
    return false;
}

enum ResourceType Match_CheckResources(Nation* nation, UnitType type)
{
    // Check resource amounts
    for (int i = 0; i < _ResourceType_Length; i++) {
        if (nation->resources[i] < nation->costs[i][type]) {
            return i;
        }
    }
    return -1;
}

void Match_DeductResources(Scene* scene, Nation* nation, UnitType type)
{
    ResourceAccepter* capitalAccepter = (ResourceAccepter*)Scene_GetComponent(scene, nation->capital, RESOURCE_ACCEPTER_COMPONENT_ID);
    for (int i = 0; i < _ResourceType_Length; i++) {
        nation->resources[i] -= nation->costs[i][type];
        capitalAccepter->storage[i] -= nation->costs[i][type];
    }
}

/*
	Takes in a producer, and a unit. Depending on the resources of the producer's
	nation, either sets the producer's order, or does nothing. */
bool Match_PlaceOrder(Scene* scene, Nation* nation, Producer* producer, Expansion* expansion, UnitType type)
{
    // Check that home city has airfield before being able to build a plane
    if (type == UnitType_FIGHTER || type == UnitType_ATTACKER || type == UnitType_BOMBER) {
        City* homeCity = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
        if (!Match_CityHasType(scene, homeCity, UnitType_AIRFIELD)) {
            if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                Match_AddMessage(errorColor, "Factory's city must have an airfield to build airplanes");
            }
            return false;
        }
    }
    if (Match_CheckResources(nation, type) != -1) {
        return false;
    }
    Match_DeductResources(scene, nation, type);
    // Set order duration
    producer->orderTicksTotal = 2 * nation->costs[ResourceType_COIN][type] * ticksPerLabor;
    producer->orderTicksRemaining = producer->orderTicksTotal;
    producer->order = type;
    nation->prodCount[type]++;
    return true;
}

CardinalDirection Match_FindDir(Vector diff)
{
    if (diff.y > 0) {
        return N;
    } else if (diff.x < 0) {
        return E;
    } else if (diff.y < 0) {
        return S;
    } else if (diff.x > 0) {
        return W;
    } else {
        return N; // Um, lol?
    }
}

/*
	Checks whether a city can be built for a nation at a position. If so, creates
	a new city, and deducts the approriate resources.
	
	Returns whether a city was/can be built */
bool Match_BuyCity(struct scene* scene, Nation* nation, Vector pos)
{
    pos.x = (float)floor(pos.x / 64) * 64 + 32;
    pos.y = (float)floor(pos.y / 64) * 64 + 32;
    enum ResourceType missingResource = Match_CheckResources(nation, UnitType_CITY);
    if (Terrain_GetHeightForBuilding(terrain, (int)pos.x, (int)pos.y) <= 0.5) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build a city on a water tile");
    } else if (Terrain_ClosestMaskDist(scene, CITY_COMPONENT_ID, terrain, (int)pos.x, (int)pos.y) <= 2) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Too close to another city");
    } else if (Terrain_ClosestBuildingDist(terrain, (int)pos.x, (int)pos.y) == 0) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build a city on top of another building");
    } else if (missingResource != -1) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Not enough resources for a city");
    } else {
        char nameBuffer[10];
        memset(nameBuffer, 0, 10);
        Lexicon_GenerateWord(lexicon, nameBuffer, 15);
        EntityID city = City_Create(scene, (Vector) { pos.x, pos.y }, nation, nameBuffer, false);
        Terrain_SetBuildingAt(terrain, city, (int)pos.x, (int)pos.y);
        Match_DeductResources(scene, nation, UnitType_CITY);
        nation->unitCount[UnitType_CITY]++;
        nation->costs[ResourceType_COIN][UnitType_CITY] *= 2;
        nation->costs[ResourceType_TIMBER][UnitType_CITY] += 5 * nation->unitCount[UnitType_CITY];
        nation->resources[ResourceType_POPULATION]++;
        return true;
    }
    return false;
}

bool Match_BuyExpansion(struct scene* scene, UnitType type, Nation* nation, Vector pos)
{
    pos.x = (float)floor(pos.x / 64) * 64 + 32;
    pos.y = (float)floor(pos.y / 64) * 64 + 32;

    char name[32];
    memset(name, 0, 32);
    Match_CopyUnitName(type, name);
    enum ResourceType missingResource = Match_CheckResources(nation, type);
    EntityID homeCity = Terrain_AdjacentMask(scene, CITY_COMPONENT_ID, terrain, (int)pos.x, (int)pos.y);
    if (type != UnitType_FARM && nation->resources[ResourceType_POPULATION] - (nation->unitCount[UnitType_FARM] + nation->unitCount[UnitType_CITY]) >= 5 * nation->unitCount[UnitType_FARM]) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Not enough food");
    } else if (type != UnitType_PORT && Terrain_GetHeightForBuilding(terrain, (int)pos.x, (int)pos.y) <= 0.5) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build %s on a water tile", name);
    } else if (type == UnitType_PORT && Terrain_GetHeightForBuilding(terrain, (int)pos.x, (int)pos.y) > 0.5) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build %s on a land tile", name);
    } else if (homeCity == INVALID_ENTITY_INDEX) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "%s must be adjacent to a city", name);
    } else if (Terrain_GetBuildingAt(terrain, (int)pos.x, (int)pos.y) != INVALID_ENTITY_INDEX) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build %s on top of another building", name);
    } else if (missingResource != -1) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Not enough resources for a %s", name);
    } else {
        Sprite* homeCitySprite = (Sprite*)Scene_GetComponent(scene, homeCity, SPRITE_COMPONENT_ID);
        City* homeCityComponent = (City*)Scene_GetComponent(scene, homeCity, CITY_COMPONENT_ID);
        Vector diff = Vector_Scalar(Vector_Normalize(Vector_Sub(pos, homeCitySprite->pos)), -16);
        CardinalDirection dir = Match_FindDir(diff);
        EntityID building = INVALID_ENTITY_INDEX;
        switch (type) {
        case UnitType_FACTORY:
            building = Factory_Create(scene, Vector_Add(diff, pos), nation, homeCity, dir);
            break;
        case UnitType_PORT:
            building = Port_Create(scene, pos, nation, homeCity, dir);
            break;
        case UnitType_AIRFIELD:
            building = Airfield_Create(scene, Vector_Add(diff, pos), nation, homeCity, dir);
            break;
        case UnitType_FARM:
            building = Farm_Create(scene, Vector_Add(diff, pos), nation, homeCity, dir);
            break;
        case UnitType_ACADEMY:
            building = Academy_Create(scene, Vector_Add(diff, pos), nation, homeCity, dir);
            break;
        case UnitType_FOUNDRY:
            building = Foundry_Create(scene, Vector_Add(diff, pos), nation, homeCity, dir);
            break;
        case UnitType_POWERPLANT:
            building = PowerPlant_Create(scene, Vector_Add(diff, pos), nation, homeCity, dir);
            break;
        default:
            PANIC("Add the building, dumby!");
        }
        homeCityComponent->expansions[dir] = building;
        Terrain_SetBuildingAt(terrain, building, (int)pos.x, (int)pos.y);
        Match_DeductResources(scene, nation, type);
        nation->unitCount[type]++;
        nation->costs[ResourceType_COIN][type] *= 2;
        nation->costs[ResourceType_TIMBER][type] += 5 * nation->unitCount[type];
        nation->resources[ResourceType_POPULATION]++;
        return true;
    }
    return false;
}

void Match_BuyBuilding(struct scene* scene, UnitType type, Nation* nation, Vector pos)
{
    pos.x = (float)floor(pos.x / 64) * 64 + 32;
    pos.y = (float)floor(pos.y / 64) * 64 + 32;

    char name[32];
    memset(name, 0, 32);
    Match_CopyUnitName(type, name);
    enum ResourceType missingResource = Match_CheckResources(nation, type);
    if (nation->resources[ResourceType_POPULATION] - (nation->unitCount[UnitType_FARM] + nation->unitCount[UnitType_CITY]) >= 5 * nation->unitCount[UnitType_FARM]) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Not enough food");
    } else if (type != UnitType_PORT && Terrain_GetHeightForBuilding(terrain, (int)pos.x, (int)pos.y) <= 0.5) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build %s on a water tile", name);
    } else if (type == UnitType_PORT && Terrain_GetHeightForBuilding(terrain, (int)pos.x, (int)pos.y) > 0.5) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build %s on a land tile", name);
    } else if (Terrain_GetBuildingAt(terrain, (int)pos.x, (int)pos.y) != INVALID_ENTITY_INDEX) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Cannot build %s on top of another building", name);
    } else if (missingResource != -1) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID)
            Match_AddMessage(errorColor, "Not enough resources for a %s", name);
    } else {
        nation->unitCount[type]++;
        nation->resources[ResourceType_POPULATION]++;
        EntityID building = INVALID_ENTITY_INDEX;
        switch (type) {
        case UnitType_TIMBERLAND:
            building = Timberland_Create(scene, pos, nation);
            break;
        case UnitType_MINE:
            building = Mine_Create(scene, pos, nation);
            break;
        default:
            PANIC("Add the building, dumby!");
        }
        Terrain_SetBuildingAt(terrain, building, (int)pos.x, (int)pos.y);
        Match_DeductResources(scene, nation, type);
        nation->costs[ResourceType_COIN][type] *= 2;
        if (type != UnitType_TIMBERLAND) {
            nation->costs[ResourceType_TIMBER][type] += 5 * nation->unitCount[type];
        }
        return true;
    }
    return false;
}

bool Match_BuyWall(struct scene* scene, Nation* nation, Vector pos, float angle)
{
    Vector cellMidPoint = { 64.0f * (int)(pos.x / 64) + 32.0f, 64.0f * (int)(pos.y / 64) + 32.0f };
    float xOffset = cellMidPoint.x - pos.x;
    float yOffset = cellMidPoint.y - pos.y;
    enum ResourceType missingResource = Match_CheckResources(nation, UnitType_WALL);
    bool centralWall = false;

    // Check if central wall, with given angle
    if (xOffset * xOffset + yOffset * yOffset < 15 * 15) {
        if ((angle < M_PI / 4 && angle > 0) || angle > 7 * M_PI / 4 || (angle > 3 * M_PI / 4 && angle < 5 * M_PI / 4)) {
            angle = 0;
        } else {
            angle = (float)M_PI / 2;
        }
        centralWall = true;
    }

    if (centralWall && Terrain_GetBuildingAt(terrain, (int)cellMidPoint.x, (int)cellMidPoint.y) != INVALID_ENTITY_INDEX) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
            Match_AddMessage(errorColor, "Cannot build a wall on top of a building");
        }
        return false;
    } else if (Terrain_GetWallAt(terrain, (int)cellMidPoint.x, (int)cellMidPoint.y) != INVALID_ENTITY_INDEX) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
            Match_AddMessage(errorColor, "Cannot build a wall on top of another wall");
        }
        return false;
    } else if (missingResource != -1) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
            Match_AddMessage(errorColor, "Not enough resources for a wall");
        }
        return false;
    } else {
        // Upward orientation
        if (!centralWall) {
            if (fabs(xOffset) > fabs(yOffset)) {
                if (xOffset > 0) {
                    cellMidPoint.x -= 32;
                } else {
                    cellMidPoint.x += 32;
                }
                angle = (float)M_PI / 2;
            }
            // Sideways orientation
            else {
                if (yOffset > 0) {
                    cellMidPoint.y -= 32;
                } else {
                    cellMidPoint.y += 32;
                }
                angle = 0;
            }
        }

        EntityID wallID = Wall_Create(scene, cellMidPoint, angle, nation);
        Terrain_SetWallAt(terrain, wallID, (int)cellMidPoint.x, (int)cellMidPoint.y);
        Match_DeductResources(scene, nation, UnitType_WALL);
        return true;
    }
}

void Match_SetAlertedTile(Nation* nation, float x, float y, float value)
{
    if ((x + 16) / 32 < 0 || (y + 16) / 32 < 0 || (x + 16) / 32 >= nation->visitedSpacesSize || (y + 16) / 32 >= nation->visitedSpacesSize) {
        return;
    }

    float oldValue = nation->visitedSpaces[(int)((x + 16) / 32) + (int)((y + 16) / 32) * nation->visitedSpacesSize];
    if (value < 0 && value < oldValue) {
        Vector point = { (int)((x + 16) / 32), (int)((y + 16) / 32) };
        if (Arraylist_Contains(nation->highPrioritySpaces, &point)) {
            Arraylist_Remove(nation->highPrioritySpaces, Arraylist_IndexOf(nation->highPrioritySpaces, &point));
        }
        Arraylist_Add(&nation->highPrioritySpaces, &point);
    }
    nation->visitedSpaces[(int)((x + 16) / 32) + (int)((y + 16) / 32) * nation->visitedSpacesSize] = min(oldValue, value);
}

void Match_ClearVisitedSpace(Nation* nation, float x0, float y0, float radius)
{
    for (int x1 = -radius; x1 <= radius; x1 += 32) {
        for (int y1 = -radius; y1 <= radius; y1 += 32) {
            float x = x0 + x1;
            float y = y0 + y1;
            if ((x + 16) / 32 < 0 || (y + 16) / 32 < 0 || (x + 16) / 32 >= nation->visitedSpacesSize || (y + 16) / 32 >= nation->visitedSpacesSize) {
                continue;
            }
            if (sqrtf(x1 * x1 + y1 * y1) > radius) {
                continue;
            }
            float oldValue = nation->visitedSpaces[(int)((x + 16) / 32) + (int)((y + 16) / 32) * nation->visitedSpacesSize];
            Vector point = { (int)((x + 16) / 32), (int)((y + 16) / 32) };
            nation->visitedSpaces[(int)point.x + (int)point.y * nation->visitedSpacesSize] = 3600;
            if (Arraylist_Contains(nation->highPrioritySpaces, &point)) {
                Arraylist_Remove(nation->highPrioritySpaces, Arraylist_IndexOf(nation->highPrioritySpaces, &point));
            }
        }
    }
}

void Match_SetUnitEngagedTicks(Sprite* sprite, Unit* unit)
{
    if (sprite->speed == 0) {
        unit->engagedTicks = 60 * 60 * 60; // 1 hour
    } else {
        unit->engagedTicks = max(unit->engagedTicks, (int)(128.0f / sprite->speed));
    }
}

void Match_SwapAllegiance(Scene* scene, Nation* nation, Nation* newNation, EntityID id)
{
    Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
    Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

    if (unit->type != UnitType_TIMBERLAND) {
        nation->costs[ResourceType_TIMBER][unit->type] -= 5 * nation->unitCount[unit->type];
    }
    nation->costs[ResourceType_COIN][unit->type] /= 2;

    nation->unitCount[unit->type]--;
    newNation->unitCount[unit->type]++;

    // You don't get the farms of other people
    if (unit->type == UnitType_FARM) {
        unit->isDead = true;
    }

    if (Scene_EntityHasComponents(scene, id, PRODUCER_COMPONENT_ID)) {
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        if (producer->order != -1) {
            producer->orderTicksRemaining = -1;
            nation->prodCount[producer->order]--;
            newNation->prodCount[producer->order]++;
            producer->order = -1;
        }
    }

    if (Scene_EntityHasComponents(scene, id, CITY_COMPONENT_ID)) {
        City* city = (City*)Scene_GetComponent(scene, id, CITY_COMPONENT_ID);
        Arraylist_Remove(nation->cities, Arraylist_IndexOf(nation->cities, &id));
        Arraylist_Add(&newNation->cities, &id);
        city->captureNation = NULL;
        unit->health = 100;
        unit->isDead = false;
        unit->deathTicks = 0;
        if (newNation->capital == INVALID_ENTITY_INDEX) {
            Nation_ResetResources(scene, newNation);
            Nation_SetCapital(scene, newNation, id);
            city->isCapital = true;
            sprite->sprite = CAPITAL_TEXTURE_ID;
            sprite->shadow = CAPITAL_SHADOW_TEXTURE_ID;
            sprite->spriteOutline = CAPITAL_OUTLINE_TEXTURE_ID;
            if (newNation->unitCount[UnitType_ENGINEER] == 0) {
                newNation->unitCount[UnitType_ENGINEER]++; // Done first, so that engineer unit ordinal is correct
                Engineer_Create(scene, sprite->pos, newNation);
            }
        } else {
            city->isCapital = false;
            sprite->sprite = CITY_TEXTURE_ID;
            sprite->shadow = CITY_SHADOW_TEXTURE_ID;
            sprite->spriteOutline = CITY_OUTLINE_TEXTURE_ID;
        }
    }

    if (unit->focused) {
        GUI_SetShown(scene, unit->focused, false);
    }

    sprite->nation = newNation;
    Scene_Unassign(scene, id, AI_COMPONENT_ID);
    Scene_Unassign(scene, id, PLAYER_FLAG_COMPONENT_ID);
    Scene_Assign(scene, id, newNation->controlFlag, NULL);
}

static void Match_AcceptParticle(Scene* scene, enum ResuourceType resourceType, EntityID id)
{
    // TODO: Check that entity isn't dead
    Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
    ResourceAccepter* accepter = (ResourceAccepter*)Scene_GetComponent(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID);
    Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
    accepter->transit[resourceType]--;
    accepter->storage[resourceType]++;
    accepter->ticksSinceLastAccept[resourceType] = 0;

    switch (unit->type) {
    case UnitType_CITY:
        sprite->nation->resources[resourceType]++;
        break;
    case UnitType_FOUNDRY: {
        ResourceProducer* resourceProducer = (ResourceProducer*)Scene_GetComponent(scene, id, RESOURCE_PRODUCER_COMPONENT_ID);
        if (accepter->storage[ResourceType_ORE] >= 1 && accepter->storage[ResourceType_COAL] >= 1 && resourceProducer->resourceTicksRemaining == -1) {
            resourceProducer->resourceTicksRemaining = resourceProducer->resourceTicksTotal;
        }
        break;
    }
    case UnitType_POWERPLANT: {
        ResourceProducer* resourceProducer = (ResourceProducer*)Scene_GetComponent(scene, id, RESOURCE_PRODUCER_COMPONENT_ID);
        if (accepter->storage[ResourceType_COAL] >= 1 && resourceProducer->resourceTicksRemaining == -1) {
            resourceProducer->resourceTicksRemaining = resourceProducer->resourceTicksTotal;
            resourceProducer->resourceTicksRemaining = resourceProducer->resourceTicksTotal;
        }
        break;
    }
    }
}

bool Match_Board(Scene* scene, Nation* nation, EntityID boardableID)
{
    Boardable* boardable = (Boardable*)Scene_GetComponent(scene, boardableID, BOARDABLE_COMPONENT_ID);
    if (boardable->size >= 4) {
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
            Match_AddMessage(errorColor, "Full, cannot board");
        }
        return false;
    } else {
        boardable->children[boardable->size] = boardUnitID;
        boardable->size++;
        Sprite* boardingSprite = (Sprite*)Scene_GetComponent(scene, boardUnitID, SPRITE_COMPONENT_ID);
        Target* boardingTarget = (Target*)Scene_GetComponent(scene, boardUnitID, TARGET_COMPONENT_ID);
        Unit* boardingUnit = (Unit*)Scene_GetComponent(scene, boardUnitID, UNIT_COMPONENT_ID);
        if (boardingUnit->isDead) {
            if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                Match_AddMessage(errorColor, "Unit is dead");
            }
            return false;
        }
        boardingSprite->hidden = true;
        boardingUnit->boarded = true;
        boardingTarget->tar = boardingSprite->pos;

        boardUnitID = INVALID_ENTITY_INDEX;
        boardMode = false;
        instantDefocus = true; // Defoucs boarding unit's GUI
        return true;
    }
}

// SYSTEMS

void Match_AIUpdateVisitedSpaces(struct scene* scene)
{
    for (int i = 0; i < nations->size; i++) {
        Nation* nation = Arraylist_Get(nations, i);

        for (int y = 0; y < nation->visitedSpacesSize; y++) {
            for (int x = 0; x < nation->visitedSpacesSize; x++) {
                if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] > 0) {
                    nation->visitedSpaces[x + y * nation->visitedSpacesSize] -= 0.1f;
                } else if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] > -1) {
                    nation->visitedSpaces[x + y * nation->visitedSpacesSize] = 0;
                    Vector point = { x, y };
                    if (Arraylist_Contains(nation->highPrioritySpaces, &point)) {
                        Arraylist_Remove(nation->highPrioritySpaces, Arraylist_IndexOf(nation->highPrioritySpaces, &point));
                    }
                }
            }
        }
    }
}

/*
	Checks each unit entity against all projectile entites. If the two are 
	collided, the unit of the entity is reduced based on the projectile's 
	attack, and the unit's defense. 
	
	Some projectiles have splash damage. If so, the damage of the projectile 
	falls off as the distance from the projectile increases 
	
	If the entity is a wall or building, and is destroyed, will remove from 
	either the wall or building map in the terrain struct */
void Match_Unit(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = sprite->nation;

        if (unit->isDead) {
            if (unit->deathTicks < 16) {
                unit->deathTicks++;
            } else {
                City* homeCity = NULL;
                if (!Scene_EntityHasComponents(scene, id, CITY_COMPONENT_ID)) {
                    Scene_MarkPurged(scene, id);
                    nation->resources[ResourceType_POPULATION]--;
                } else {
                    City* city = (City*)Scene_GetComponent(scene, id, CITY_COMPONENT_ID);
                    Nation* otherNation = city->captureNation;
                    if (otherNation != NULL && rand() % 2 == 0) {
                        Match_SwapAllegiance(scene, nation, otherNation, id);
                    } else {
                        Scene_MarkPurged(scene, id);
                        Terrain_SetBuildingAt(terrain, INVALID_ENTITY_INDEX, (int)sprite->pos.x, (int)sprite->pos.y);
                        for (int i = 0; i < 4; i++) {
                            EntityID expansionID = city->expansions[i];
                            if (expansionID == INVALID_ENTITY_INDEX) {
                                continue;
                            }
                            Unit* expansion = (Unit*)Scene_GetComponent(scene, expansionID, UNIT_COMPONENT_ID);
                            expansion->isDead = true;
                        }
                    }
                }

                // Remove expansion from city's array, remove expansion from terrain map
                if (Scene_EntityHasComponents(scene, id, EXPANSION_COMPONENT_ID)) {
                    Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);
                    homeCity = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
                    homeCity->expansions[expansion->dir] = INVALID_ENTITY_INDEX;
                }

                if (Scene_EntityHasComponents(scene, id, BUILDING_FLAG_COMPONENT_ID)) {
                    Terrain_SetBuildingAt(terrain, INVALID_ENTITY_INDEX, (int)sprite->pos.x, (int)sprite->pos.y);
                    nation->costs[ResourceType_COIN][unit->type] /= 2;
                    if (unit->type != UnitType_TIMBERLAND) {
                        nation->costs[ResourceType_TIMBER][unit->type] -= 5 * nation->unitCount[unit->type];
                    }
                }

                if (Scene_EntityHasComponents(scene, id, UNIT_COMPONENT_ID)) {
                    Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
                    if (unit->focused) {
                        unit->focused = false;
                        instantDefocus = true;
                    }
                }
                if (Scene_EntityHasComponents(scene, id, PRODUCER_COMPONENT_ID)) {
                    Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
                    if (producer->order != -1) {
                        nation->prodCount[producer->order]--;
                    }
                }

                if (unit->type) {
                    Terrain_SetWallAt(terrain, INVALID_ENTITY_INDEX, (int)sprite->pos.x, (int)sprite->pos.y);
                }
                nation->unitCount[unit->type]--;
            }
        } else if (!unit->boarded) {
            if (sprite->hitTicks > 0) {
                sprite->hitTicks--;
            }
            if (boardMode && boardUnitID != INVALID_ENTITY_INDEX) {
                Sprite* boardSprite = (Sprite*)Scene_GetComponent(scene, boardUnitID, SPRITE_COMPONENT_ID);
                int boardTile = (int)(boardSprite->pos.x / 64) + (int)(boardSprite->pos.y / 64) * terrain->tileSize;
                int unitTile = (int)(sprite->pos.x / 64) + (int)(sprite->pos.y / 64) * terrain->tileSize;
                if (boardTile != unitTile || !Scene_EntityHasComponents(scene, id, BOARDABLE_COMPONENT_ID)) {
                    unit->deathTicks = 8;
                } else {
                    unit->deathTicks = 0;
                }
            } else {
                unit->deathTicks = 0;
            }
            unit->aliveTicks++;

            // Find closest enemy projectile
            Nation* otherNation = NULL;
            Sprite* otherSprite = NULL;
            system(scene, otherID, SPRITE_COMPONENT_ID, PROJECTILE_COMPONENT_ID)
            {
                if (!Scene_EntityHasAnyComponents(scene, unit->sensedProjectiles, otherID)) {
                    continue;
                }
                otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
                if (!Arraylist_Contains(nation->enemyNations, &otherSprite->nation)) {
                    continue;
                }
                Projectile* projectile = (Projectile*)Scene_GetComponent(scene, otherID, PROJECTILE_COMPONENT_ID);
                otherNation = otherSprite->nation;

                float dist = Vector_Dist(sprite->pos, otherSprite->pos);
                if (projectile->armed && dist < projectile->splash) {
                    float splashDamageModifier;
                    if (projectile->splash <= 8) {
                        splashDamageModifier = 1.0f; // Damage is same regardless of distance
                    } else {
                        splashDamageModifier = 1.0f - dist / projectile->splash; // The farther away from splash damage, the less damage it does
                    }
                    unit->health -= projectile->attack * splashDamageModifier / unit->defense;
                    // Building set engaged ticks, visited spaces (building defense should be top priority)
                    if (Scene_EntityHasComponents(scene, id, BUILDING_FLAG_COMPONENT_ID) || Scene_EntityHasComponents(scene, id, WALL_FLAG_COMPONENT_ID)) {
                        Match_SetAlertedTile(nation, sprite->pos.x, sprite->pos.y, -10);
                        if (otherNation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                            Match_SetUnitEngagedTicks(sprite, unit);
                        }
                    }
                    // GROUND UNITS: Do more damage if flanked
                    if (sprite->z == 0.5f && Scene_EntityHasComponents(scene, id, TARGET_COMPONENT_ID)) {
                        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
                        Vector displacement = Vector_Sub(sprite->pos, otherSprite->pos); // From other to me
                        unit->health -= projectile->attack * 10.0f * (Vector_Dot(Vector_Normalize(displacement), Vector_Normalize(Vector_Sub(target->lookat, sprite->pos))) + 1.0f);
                    }
                    sprite->hitTicks = 18;
                    Scene_MarkPurged(scene, otherID);

                    // If dead don't bother checking the rest of the particles
                    if (unit->health <= 0) {
                        unit->isDead = true;
                        if (Scene_EntityHasComponents(scene, id, CITY_COMPONENT_ID)) {
                            City* city = (City*)Scene_GetComponent(scene, id, CITY_COMPONENT_ID);
                            city->captureNation = otherSprite->nation;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void Match_CheckWin(Scene* scene)
{
#ifdef TOURNAMENT
    if (Apricot_Ticks > 30 * 60 * 60) {
        printf("Overtime\n");
        Game_PopScene(1);
        return;
    }
#endif
    for (int i = 0; i < nations->size; i++) {
        Nation* nation = (Nation*)Arraylist_Get(nations, i);
        if (nation->capital == INVALID_ENTITY_INDEX) {
            continue;
        }
        Sprite* capitalRender = (Scene*)Scene_GetComponent(scene, nation->capital, SPRITE_COMPONENT_ID);
        if (capitalRender->nation != nation) {
#ifdef TOURNAMENT
            if (nation->ownNationFlag == HOME_NATION_FLAG_COMPONENT_ID) {
                printf("Defeat\n");
            } else {
                printf("Victory\n");
            }
            Game_PopScene(1);
            return;
#else
            if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                Pause_Init(scene, DEFEAT);
            } else {
                //Pause_Init(scene, VICTORY);
                nation->capital = INVALID_ENTITY_INDEX;
            }
#endif
        }
    }
}

/*
	Takes in a scene, iterates through all entites that have a sprite component. 
	Their position is then incremented by their velocity. */
void Match_SpriteMove(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Vector newPos = Vector_Add(sprite->pos, sprite->vel);
        sprite->dZ += sprite->aZ;
        sprite->z += sprite->dZ;
        float height = Terrain_GetHeight(terrain, (int)sprite->pos.x, (int)sprite->pos.y);
        sprite->pos = Vector_Add(sprite->pos, sprite->vel);
        if (sprite->destroyOnBounds && height == -1) {
            Scene_MarkPurged(scene, id);
        }
    }
}

/*
	Goes through each nation, updates the visited spaces array.
	
	The array represents an AI nation's confidence that there are no enemy units
	at the position of each sub tile. This system makes nations more unsure as time
	passes*/
void Match_SetVisitedSpace(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Combatant* combatant = (Combatant*)Scene_GetComponent(scene, id, COMBATANT_COMPONENT_ID);
        Nation* nation = sprite->nation;

        if (!unit->engaged && sprite->speed > 0 && !unit->boarded) {
            Match_ClearVisitedSpace(nation, sprite->pos.x, sprite->pos.y, combatant->attackDist);
        }
    }
}

/*
	Takes in a scene, iterates through all entities that have a target component.
	First checks that the angle is correct, and then sets the velocity according
	to the target vector accordingly
	
	Stops units if they go through an enemy wall */
void Match_Target(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        // Calculate if pointing in direction of lookat vector
        Vector displacement = Vector_Sub(sprite->pos, target->lookat);
        float tempAngle = Vector_Angle(displacement); // atan2
        if (fabs(sprite->angle - tempAngle) < 3.1415926) {
            // Can be compared directly
        } else if (sprite->angle < tempAngle) {
            sprite->angle += 2 * 3.1415926f;
        } else if (sprite->angle > tempAngle) {
            sprite->angle -= 2 * 3.1415926f;
        }
        float diff = (float)fabs(sprite->angle - tempAngle);

        if (diff > sprite->speed / 18.0f) {
            // Not looking in direction, turn
            if (sprite->angle > tempAngle) {
                sprite->angle -= 5.0f * powf(sprite->speed, 2) / 18.0f;
            } else {
                sprite->angle += 5.0f * powf(sprite->speed, 2) / 18.0f;
            }
            sprite->vel.x = 0;
            sprite->vel.y = 0;
        } else if (Vector_Dist(target->tar, sprite->pos) > sprite->speed) {
            // Looking in direction, not at target, move
            sprite->vel.x = (target->tar.x - sprite->pos.x);
            sprite->vel.y = (target->tar.y - sprite->pos.y);
            float mag = sqrtf((sprite->vel.x * sprite->vel.x) + (sprite->vel.y * sprite->vel.y));
            float slownessFactor = sprite->z == 0.5 ? -Terrain_GetHeight(terrain, (int)sprite->pos.x, (int)sprite->pos.y) / 2.0f + 1.25f : 1.0f;
            if (mag > 0.1) {
                sprite->vel.x /= mag / (slownessFactor * sprite->speed);
                sprite->vel.y /= mag / (slownessFactor * sprite->speed);
            }

            displacement = Vector_Add((sprite->pos), (sprite->vel));
            float terrainHeight = Terrain_GetHeight(terrain, (int)displacement.x, (int)displacement.y);
            if (sprite->z <= 0.5f) {
                // Not in air and hit edge -> stay still
                if (terrainHeight < sprite->z || terrainHeight > sprite->z + 0.5f) {
                    sprite->vel.x = 0;
                    sprite->vel.y = 0;
                    target->tar = sprite->pos;
                }

                // Check for enemy walls
                Nation* nation = sprite->nation;
                system(scene, wallID, WALL_FLAG_COMPONENT_ID)
                {
                    // Can't pass through the walls of other nations (even if friendly)
                    Sprite* wallSprite = (Sprite*)Scene_GetComponent(scene, wallID, SPRITE_COMPONENT_ID);
                    if (wallSprite->nation == sprite->nation) {
                        continue;
                    }
                    float beforeDiff = 0;
                    float afterDiff = 0;
                    if (wallSprite->angle != 0 && sprite->pos.y < wallSprite->pos.y + 32 && sprite->pos.y > wallSprite->pos.y - 32) {
                        beforeDiff = sprite->pos.x - wallSprite->pos.x;
                        afterDiff = sprite->pos.x - wallSprite->pos.x + sprite->vel.x;
                    } else if (sprite->pos.x < wallSprite->pos.x + 32 && sprite->pos.x > wallSprite->pos.x - 32) {
                        beforeDiff = sprite->pos.y - wallSprite->pos.y;
                        afterDiff = sprite->pos.y - wallSprite->pos.y + sprite->vel.y;
                    }
                    if (beforeDiff < 0 && afterDiff > 0 || beforeDiff > 0 && afterDiff < 0) {
                        sprite->vel.x = 0;
                        sprite->vel.y = 0;
                    }
                }
            }
        } else {
            sprite->vel.x = 0;
            sprite->vel.y = 0;
        }

        while (sprite->angle > M_PI * 2) {
            sprite->angle -= (float)M_PI * 2.0f;
        }
        while (sprite->angle < 0) {
            sprite->angle += (float)M_PI * 2.0f;
        }
    }
}

/*
fly towards enemy
if enemy is behind you, keep flying until you're far enough away, then turn around
 0.9, 1.0: Go straight
 0.0, 0.9: Fly towards target
-1.0, 0.0: Turn around if you're far enough away
*/
void Match_Patrol(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, PATROL_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        Vector innerCircle = Vector_Normalize(Vector_Sub(sprite->pos, patrol->focalPoint)); // Points from patrol to pos
        Vector perpVel = { -sprite->vel.y, sprite->vel.x };
        float targetAlignment = Vector_Dot(Vector_Normalize(sprite->vel), innerCircle);

        // Only make targetAlignment better
        if (targetAlignment < 0) {
            float diff = Vector_Dot(perpVel, innerCircle);
            diff *= 0.5f;
            diff += diff >= 0 ? 0.5f : -0.5f;
            // If perpVel and innerCircle are perpendicular (dot == 0), then you're right on track.
            // dot is + -> turn left -> increase angle
            // dot is - -> turn right -> decrease angle
            patrol->angle += sprite->speed * diff / 20.0f;
        }
        // If not going directly away, and distance is greater than 72
        else if (targetAlignment >= 0 && Vector_Dist(sprite->pos, patrol->focalPoint) > 72) {
            patrol->angle += sprite->speed / 20.0f;
        }

        target->tar = Vector_Add(sprite->pos, Vector_Scalar((Vector) { sinf(patrol->angle), cosf(patrol->angle) }, 2 * sprite->speed));
        target->lookat = target->tar;

        Vector displacement = Vector_Sub(sprite->pos, target->lookat);
        sprite->angle = Vector_Angle(displacement); // atan2
    }
}

/*
TODO: combine this with shells so that any time an aloft projectile hits the ground it becomes armed.
*/
void Match_BombMove(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, PROJECTILE_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Projectile* projectile = (Projectile*)Scene_GetComponent(scene, id, PROJECTILE_COMPONENT_ID);

        if (sprite->z < 0.5) {
            if (!projectile->armed) {
                projectile->armed = true;
            } else {
                Scene_MarkPurged(scene, id);
            }
        }
    }
}

/*
	Calculates whether a unit is hovered over with the mouse. Assumes rectangle 
	shape, given by Sprite texture bounds */
void Match_Hover(struct scene* scene)
{
    static bool drawingBox = false;
    bool heldDown = Apricot_MouseDrag && Apricot_Keys[SDL_SCANCODE_LSHIFT];
    Vector mouse = Terrain_MousePos();
    if (drawingBox && !heldDown) {
        drawingBox = false;
        selectBox = true;
    } else if (heldDown) {
        if (!drawingBox) {
            boxTL = mouse;
        }
        boxBR = mouse;
        drawingBox = true;
    } else {
        boxTL = (Vector) { -1, -1 };
        selectBox = false;
    }

    bool anySelected = false;
    system(scene, id, TARGET_COMPONENT_ID)
    {
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        anySelected |= target->selected;
    }

    EntityID hoveredID = INVALID_ENTITY_INDEX;
    enum RenderPriority priority = RenderPriorirty_BUILDING_LAYER;
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        unit->isHovered = false;
        sprite->showOutline = false;
        if (unit->boarded) {
            continue;
        }

        if (selectBox) {
            unit->isHovered = sprite->pos.x > boxTL.x && sprite->pos.x < boxBR.x && sprite->pos.y > boxTL.y && sprite->pos.y < boxBR.y;
        } else if (sprite->priority >= priority) {
            float dx = sprite->pos.x - mouse.x;
            float dy = mouse.y - sprite->pos.y + (sprite->z < 0.5 ? 0 : 60 * sprite->z - 28);

            float sin = sinf(sprite->angle);
            float cos = cosf(sprite->angle);

            bool checkLR = fabs(sin * dx + cos * dy) <= sprite->height / 2;
            bool checkTB = fabs(cos * dx - sin * dy) <= sprite->width / 2;
            bool checkBoard = true;

            if (boardMode) {
                Sprite* boardSprite = (Sprite*)Scene_GetComponent(scene, boardUnitID, SPRITE_COMPONENT_ID);
                int boardTile = (int)(boardSprite->pos.x / 64) + (int)(boardSprite->pos.y / 64) * terrain->tileSize;
                int unitTile = (int)(sprite->pos.x / 64) + (int)(sprite->pos.y / 64) * terrain->tileSize;
                checkBoard = (boardTile == unitTile && Scene_EntityHasComponents(scene, id, BOARDABLE_COMPONENT_ID));
            }

            if (checkLR && checkTB && checkBoard) {
                hoveredID = id;
                priority = sprite->priority;
            }
        }
    }
    if (hoveredID != INVALID_ENTITY_INDEX) {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, hoveredID, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, hoveredID, UNIT_COMPONENT_ID);
        unit->isHovered = true;
        sprite->showOutline = !anySelected || Apricot_Keys[SDL_SCANCODE_LCTRL];
    }
}

/*
	Checks to see if the escape key is pressed, and if it is, clears all focused 
	units */
void Match_EscapePressed(struct scene* scene)
{
    static bool escDown = false;
    if (Apricot_Keys[SDL_SCANCODE_ESCAPE]) {
        if (!escDown) {
            if (boardMode) {
                boardMode = false;
            } else {
                bool anyFlag = false;

                system(scene, id, TARGET_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
                {
                    Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
                    anyFlag |= target->selected;
                    target->selected = false;
                }

                system(scene, id, UNIT_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
                {
                    Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
                    anyFlag |= unit->focused;
                    unit->focused = false;
                    escFocus = true;
                }

                if (!anyFlag) {
                    Pause_Init(scene, PAUSE);
                }
            }
        }
        escDown = true;
    } else {
        escDown = false;
    }
}

/*
	 Iterates through entities that are target. Determines if a unit is 
	 hovered, selected, if a whole task force is selected, and if and where to 
	 set units' targets. */
void Match_Select(struct scene* scene)
{
    bool targeted = false;
    // If ctrl is not clicked, go through entities, if they are selected, set their target
    // ! CTRL DETERMINES IF NEXT CLICK IS SELECT OR TARGET
    if (!Apricot_Keys[SDL_SCANCODE_LCTRL] && Apricot_MouseLeftUp && Vector_Magnitude(Vector_Sub(Apricot_MouseInit, Apricot_MousePos)) < 16) {
        Vector centerOfMass = { 0, 0 };
        // If shift is held down, find center of mass of selected units
        if (Apricot_Keys[SDL_SCANCODE_LSHIFT]) {
            int numSelected = 0;
            system(scene, id, TARGET_COMPONENT_ID, TARGET_COMPONENT_ID, SPRITE_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
            {
                Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
                Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
                if (target->selected) {
                    centerOfMass = Vector_Add(centerOfMass, sprite->pos);
                    numSelected++;
                }
            }
            if (numSelected != 0) {
                centerOfMass = Vector_Scalar(centerOfMass, 1.0f / numSelected);
            }
        }
        system(scene, id, TARGET_COMPONENT_ID, SPRITE_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
        {
            Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
            Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
            if (target->selected) {
                Vector mouse = Terrain_MousePos();
                if (Apricot_Keys[SDL_SCANCODE_LSHIFT]) { // Offset by center of mass, calculated earlier
                    Vector distToCenter = Vector_Sub(sprite->pos, centerOfMass);
                    mouse = Vector_Add(mouse, distToCenter);
                }
                if (Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID)) {
                    Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
                    patrol->patrolPoint = mouse;
                } else {
                    target->tar = mouse;
                    target->lookat = mouse;
                }
                if (!Apricot_Keys[SDL_SCANCODE_S]) { // Check if should (s)tandby for more orders
                    target->selected = false;
                }
                targeted = true;
            }
        }
    }

    Target* hovered = NULL;
    // If no unit targets were set previously
    if (!targeted) {
        // Go thru entities, check to see if they are now hovered and selected
        system(scene, id, TARGET_COMPONENT_ID, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
        {
            Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
            Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
            Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);

            if (target->selected) {
                sprite->showOutline = 2;
            }
            if (unit->isHovered && Apricot_MouseLeftUp) {
                if (boardMode && Scene_EntityHasComponents(scene, id, BOARDABLE_COMPONENT_ID)) {
                    if (Match_Board(scene, sprite->nation, id)) {
                        break;
                    }
                } else {
                    target->selected = !target->selected;
                    if (!selectBox) { // Only select multiple units if box-selecting
                        break;
                    }
                }
            }
        }
    }
}

/*
	When the right mouse button is released, finds the unit entity that
	is focused, and shows its GUI. */
void Match_Focus(struct scene* scene)
{
    static bool disappear = false; // Whether or not the current GUI should disappear
    static EntityID currShownGUI = INVALID_ENTITY_INDEX;
    static EntityID currFocusedGUI = INVALID_ENTITY_INDEX;
    static EntityID currFocusedEntity = INVALID_ENTITY_INDEX; // The ONLY entity that is currently focused, or none
    static bool focusedIsProducer = false;
    static UnitType type;
    static int x = 0;

    if (Apricot_MouseRightUp) {
        guiChange = false;
        Unit* unitComp = NULL;
        currFocusedGUI = INVALID_ENTITY_INDEX;
        currFocusedEntity = INVALID_ENTITY_INDEX;
        enum RenderPriority priority = RenderPriorirty_BUILDING_LAYER;

        // Find the unit that is hovered with the highest priority
        system(scene, id, UNIT_COMPONENT_ID, SPRITE_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
        {
            Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
            Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
            unit->focused = false;
            if (unit->isHovered && sprite->priority >= priority) {
                currFocusedGUI = unit->guiContainer;
                currFocusedEntity = id;
                unitComp = unit;
                type = unit->type;
                priority = sprite->priority;
            }
        }
        if (unitComp != NULL) {
            unitComp->focused = true;
        }
        if (currFocusedEntity != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, currFocusedEntity, PRODUCER_COMPONENT_ID)) {
            focusedIsProducer = true;
        } else {
            focusedIsProducer = false;
        }
        escFocus = false;
    } else if (guiChange) {
        focusedIsProducer = false;
        system(scene, id, UNIT_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
        {
            Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
            if (unit->focused) {
                currFocusedGUI = unit->guiContainer;
                if (Scene_EntityHasComponents(scene, id, PRODUCER_COMPONENT_ID)) {
                    focusedIsProducer = true;
                }
            }
        }
    }
    if (instantDefocus || escFocus) {
        currFocusedGUI = INVALID_ENTITY_INDEX;
        currFocusedEntity = INVALID_ENTITY_INDEX;
    }
    if ((Apricot_MouseRightUp && currFocusedEntity != currShownEntity) || instantDefocus || escFocus) {
        disappear = true;
        boardMode = false;
    }

    // Fade down/maintain down
    if (disappear) {
        if (x == 0) {
            disappear = false;
            instantDefocus = false;
            if (currShownGUI != INVALID_ENTITY_INDEX) {
                GUI_SetShown(scene, currShownGUI, false);
            }
            if (currFocusedGUI != INVALID_ENTITY_INDEX) {
                GUI_SetShown(scene, currFocusedGUI, true);
            }
            currShownGUI = currFocusedGUI;
            currShownEntity = currFocusedEntity;

            GUI_SetShown(scene, orderLabel, focusedIsProducer);
            GUI_SetShown(scene, timeLabel, focusedIsProducer);
            GUI_SetShown(scene, autoReOrderRockerSwitch, focusedIsProducer);
        } else {
            x--;
        }
    }
    // Fade up/maintain up
    else if (currShownGUI != INVALID_ENTITY_INDEX) {
        if (x < 12) {
            x++;
        }
        if (currShownGUI != currFocusedGUI) {
            GUI_SetShown(scene, currShownGUI, false);
            GUI_SetShown(scene, currFocusedGUI, true);
            currShownGUI = currFocusedGUI;
        }
    }

    if (currShownGUI != INVALID_ENTITY_INDEX) {
        Container* gui = (Container*)Scene_GetComponent(scene, focusedGUIContainer, GUI_CONTAINER_COMPONENT_ID);
        gui->maxWidth = Apricot_Width - 250;
        GUI_UpdateLayout(scene, focusedGUIContainer, miniMapSize + 2, Apricot_Height - 198 + 200.0f * pow((12 - x) / 12.0f, 2));
    } else {
        GUI_UpdateLayout(scene, focusedGUIContainer, miniMapSize + 2, Apricot_Height + 2);
    }
}

void Match_AI(Scene* scene)
{
    for (int i = 0; i < nations->size; i++) {
        Nation* nation = (Nation*)Arraylist_Get(nations, i);
        if (nation->controlFlag == AI_COMPONENT_ID && nation->capital != INVALID_ENTITY_INDEX) {
            Goap_Update(scene, &(nation->goap), nation);
        }
    }
    AI_TargetGroundUnitsRandomly(scene);
}

/*
	This system goes through all combatants, has them search for the closest 
	enemy to them, and finally shoot a projectile at them. */
void Match_CombatantAttack(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID)
    {
        // Exclude planes
        if (Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID)) {
            continue;
        }
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Combatant* combatant = (Combatant*)Scene_GetComponent(scene, id, COMBATANT_COMPONENT_ID);
        Nation* nation = sprite->nation;

        if (unit->isDead || unit->boarded) {
            continue;
        }

        // Find closest enemy ground unit
        float closestDist = combatant->attackDist;
        EntityID closest = INVALID_ENTITY_INDEX;
        Vector closestPos = { -1, -1 };
        Vector closestVel = { -1, -1 };
        bool groundUnit = false;
        bool onlyBuildings = true;
        Nation* otherNation = NULL;
        system_mask(scene, otherID, combatant->enemyMask)
        {
            if (!onlyBuildings && Scene_EntityHasComponents(scene, otherID, BUILDING_FLAG_COMPONENT_ID)) {
                continue;
            }
            Sprite* otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
            if (!Arraylist_Contains(nation->enemyNations, &otherSprite->nation)) {
                continue;
            }
            Unit* otherUnit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);
            if (otherUnit->isDead || otherUnit->boarded) {
                continue;
            }
            float dist = Vector_Dist(otherSprite->pos, sprite->pos);
            bool isWall = Scene_EntityHasComponents(scene, otherID, WALL_FLAG_COMPONENT_ID); // Combatants should get closer to walls
            if (isWall && Scene_EntityHasComponents(scene, id, SHELL_ATTACK_FLAG_COMPONENT_ID)) { // Artillery shoots over walls
                dist *= 2;
            }

            // Mark out enemies
            bool withinRange = ((isWall && dist < closestDist / 3) || (!isWall && dist < closestDist));
            if (dist < combatant->attackDist && sprite->z <= 0.5f) {
                for (int x = -2; x <= 2; x++) {
                    for (int y = -2; y <= 2; y++) {
                        Match_SetAlertedTile(nation, otherSprite->pos.x + x, otherSprite->pos.y + y, 0);
                    }
                }
            }

            bool isBuilding = Scene_EntityHasComponents(scene, otherID, BUILDING_FLAG_COMPONENT_ID) || Scene_EntityHasComponents(scene, otherID, WALL_FLAG_COMPONENT_ID);
            if (withinRange || (dist < combatant->attackDist && onlyBuildings && !isBuilding)) {
                // Buildings are the lowest priority. Prioritirize other units.
                if (onlyBuildings && !isBuilding) {
                    onlyBuildings = false;
                }
                closestDist = dist;
                closest = otherID;
                otherNation = otherSprite->nation;
                closestPos = otherSprite->pos;
                closestVel = otherSprite->vel;
                groundUnit = Scene_EntityHasComponents(scene, otherID, GROUND_UNIT_FLAG_COMPONENT_ID);
            }
        }

        // If no enemy units were found, stuckin and engaged are false, skip
        if (closest == INVALID_ENTITY_INDEX) {
            if (unit->engaged && Terrain_LineOfSight(terrain, sprite->pos, target->lookat, sprite->z)) {
                target->tar = target->lookat; // This causes units to lerch forward after defeating an enemy, is honestly useful
            }
            unit->engaged = false;
            continue;
        }

        Vector lead = closestPos;
        // Set flags indicating that unit is engaged in battle
        if (Scene_EntityHasComponents(scene, id, AI_COMPONENT_ID) || Vector_Dot(Vector_Normalize(Vector_Sub(target->tar, sprite->pos)), Vector_Normalize(Vector_Sub(closestPos, sprite->pos))) > 0) {
            if (groundUnit && combatant->faceEnemy) {
                target->tar = sprite->pos;
            }
            if (combatant->faceEnemy) {
                if (Scene_EntityHasComponents(scene, id, AI_COMPONENT_ID)) {
                    target->tar = sprite->pos;
                }
                target->lookat = lead;
            }
        }
        unit->engaged = true;
        unit->engagedLevel = 0;
        unit->knownByEnemy = true;
        if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID || (otherNation != NULL && otherNation->controlFlag == PLAYER_FLAG_COMPONENT_ID)) {
            Match_SetUnitEngagedTicks(sprite, unit);
        }

        // Shoot enemy units if found
        Vector displacement = Vector_Sub(sprite->pos, closestPos);
        float deflection = Vector_Angle(displacement);

        while (deflection > M_PI * 2) {
            deflection -= (float)M_PI * 2;
        }
        while (deflection < 0) {
            deflection += (float)M_PI * 2;
        }
        if (unit->aliveTicks % combatant->attackTime == 0 && (fabs(deflection - sprite->angle) < 0.2 * sprite->speed || !combatant->faceEnemy)) {
            float homeFieldAdvantage = 1.0f; //0.6 * (Vector_Dist(capital->pos, sprite->pos) / sqrtf(terrain->size * terrain->size)) + 1;
            float manPower = 1.0f; //unit->unit / 100.0f;
            combatant->projConstructor(scene, sprite->pos, lead, manPower * homeFieldAdvantage * combatant->attack, sprite->nation);
        }
    }
}

/*
	Actually just for fighters and attackers really */
void Match_AirplaneAttack(Scene* scene)
{
    system(scene, id, TARGET_COMPONENT_ID, PATROL_COMPONENT_ID, AIRCRAFT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
        Combatant* combatant = (Combatant*)Scene_GetComponent(scene, id, COMBATANT_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Nation* otherNation = NULL;

        if (unit->isDead || unit->boarded) {
            continue;
        }

        // Find closest enemy ground unit
        float closestDist = combatant->attackDist + 100.0f;
        EntityID closest = INVALID_ENTITY_INDEX;
        Vector closestPos = { -1, -1 };
        Vector closestVel = { -1, -1 };
        float closestZ = -1;
        system_mask(scene, otherID, combatant->enemyMask)
        {
            Sprite* otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
            if (!Arraylist_Contains(nation->enemyNations, &otherSprite->nation)) {
                continue;
            }
            Unit* otherUnit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);
            float patrolDist = Vector_Dist(otherSprite->pos, patrol->patrolPoint);

            if (otherUnit->isDead || otherUnit->boarded) {
                continue;
            }

            float dist = Vector_Dist(otherSprite->pos, sprite->pos);
            Vector innerCircle = Vector_Normalize(Vector_Sub(otherSprite->pos, sprite->pos)); // Points from pos to patrol
            float targetAlignment = Vector_Dot(Vector_Normalize(sprite->vel), innerCircle);

            if (patrolDist + otherUnit->health < closestDist && dist < combatant->attackDist) {
                closestDist = dist + otherUnit->health;
                closest = otherID;
                otherNation = otherSprite->nation;
                closestPos = otherSprite->pos;
                closestVel = otherSprite->vel;
                closestZ = otherSprite->z;
            }
        }

        // If no enemy units were found, stuckin and engaged are false, skip
        if (closest == INVALID_ENTITY_INDEX) {
            patrol->focalPoint = patrol->patrolPoint;
        } else if (unit->aliveTicks % combatant->attackTime == 0) {
            // Find lead angle
            /*
			Special thanks to: https://www.gamedev.net/forums/topic/457840-calculating-target-lead/4020764/
			Solve quadratic: ((P - O) + V * t)^2 - (w*w) * t^2 = 0
			*/
            Vector toEnemy = Vector_Sub(closestPos, sprite->pos);
            float a = Vector_Dot(closestVel, closestVel) - 16;
            float b = Vector_Dot(toEnemy, closestVel) * 2.0f;
            float c = Vector_Dot(toEnemy, toEnemy);
            float d = b * b - 4 * a * c;

            double t0 = (-b - sqrt(d)) / (2 * a);
            double t1 = (-b + sqrt(d)) / (2 * a);
            double t = (t0 < 0.0f) ? t1 : (t1 < 0.0f) ? t0
                                                      : min(t0, t1);

            patrol->focalPoint = Vector_Add(closestPos, Vector_Scalar(closestVel, (float)t));

            if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID || (otherNation != NULL && otherNation->controlFlag == PLAYER_FLAG_COMPONENT_ID)) {
                Match_SetUnitEngagedTicks(sprite, unit);
            }

            // Shoot enemy units if found
            Vector innerCircle = Vector_Normalize(Vector_Sub(patrol->focalPoint, sprite->pos)); // Points from pos to focal
            Vector facing = Vector_Normalize(Vector_Sub(target->lookat, sprite->pos));
            float targetAlignment = Vector_Dot(facing, innerCircle);

            unit->knownByEnemy = true;

            if (targetAlignment > 0.95) {
                combatant->projConstructor(scene, sprite->pos, target->lookat, combatant->attack, sprite->nation);
            }
        }
    }
}

void Match_AirplaneScout(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, AIRCRAFT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = sprite->nation;

        if (unit->isDead || unit->boarded) {
            continue;
        }

        system(scene, otherID, UNIT_COMPONENT_ID)
        {
            Sprite* otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
            if (!Arraylist_Contains(nation->enemyNations, &otherSprite->nation)) {
                continue;
            }
            Unit* otherUnit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);
            if (otherUnit->isDead || otherUnit->boarded) {
                continue;
            }
            Nation* otherNation = otherSprite->nation;
            if (Vector_Dist(sprite->pos, otherSprite->pos) < 128) {
                if (nation->controlFlag == PLAYER_FLAG_COMPONENT_ID || otherNation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                    Match_SetUnitEngagedTicks(sprite, unit);
                    Match_SetUnitEngagedTicks(otherSprite, otherUnit);
                }
                if (!Scene_EntityHasComponents(scene, otherID, PATROL_COMPONENT_ID) && !Scene_EntityHasComponents(scene, otherID, BUILDING_FLAG_COMPONENT_ID)) {
                    unit->knownByEnemy = true;
                    Match_SetAlertedTile(nation, otherSprite->pos.x, otherSprite->pos.y, -1);
                } else {
                    if (Scene_EntityHasComponents(scene, otherID, BUILDING_FLAG_COMPONENT_ID)) {
                        otherUnit->knownByEnemy = true;
                    }
                    for (int x = -1; x <= 1; x++) {
                        for (int y = -1; y <= 1; y++) {
                            Match_SetAlertedTile(nation, otherSprite->pos.x + x, otherSprite->pos.y + y, 0);
                        }
                    }
                }
            }
        }
    }
}

void Match_ResourceParticle(Scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, RESOURCE_PARTICLE_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        ResourceParticle* resourceParticle = (ResourceParticle*)Scene_GetComponent(scene, id, RESOURCE_PARTICLE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        if (!Scene_EntityIsValid(scene, resourceParticle->accepter)) {
            Scene_MarkPurged(scene, id);
            continue;
        }

        sprite->z = -10.0f * pow(Vector_Dist(sprite->pos, resourceParticle->accepterPos) / resourceParticle->distToAccepter - 0.5f, 4) + 1;

        // Detect if at accepter
        if (Vector_Dist(sprite->pos, resourceParticle->accepterPos) < 6) {
            Scene_MarkPurged(scene, id);
            Match_AcceptParticle(scene, resourceParticle->type, resourceParticle->accepter);
        }
    }
}

/*
	Creates a resource particle every time a production period has passed */
void Match_ProduceResources(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, RESOURCE_PRODUCER_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        ResourceProducer* resourceProducer = (ResourceProducer*)Scene_GetComponent(scene, id, RESOURCE_PRODUCER_COMPONENT_ID);

        if (resourceProducer->resourceTicksRemaining == 0) {
            EntityID accepter = INVALID_ENTITY_INDEX;
            float closestDist = FLT_MAX;
            // Search through all foundries, see if there's one you can send a particle to
            system(scene, otherID, RESOURCE_ACCEPTER_COMPONENT_ID)
            {
                Sprite* otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
                if (otherSprite->nation != sprite->nation) {
                    continue;
                }
                ResourceAccepter* resourceAccepter = (ResourceAccepter*)Scene_GetComponent(scene, otherID, RESOURCE_ACCEPTER_COMPONENT_ID);
                Unit* otherUnit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);

                if (resourceAccepter->transit[resourceProducer->type] + resourceAccepter->storage[resourceProducer->type] >= resourceAccepter->capacity[resourceProducer->type]) {
                    continue;
                }

                float dist = Vector_Dist(sprite->pos, otherSprite->pos);
                if (Scene_EntityHasComponents(scene, otherID, CITY_COMPONENT_ID)) {
                    dist *= 512; // De-prioritize cities (capital)
                }
                if (dist < closestDist) {
                    closestDist = dist;
                    accepter = otherID;
                }
            }

            if (accepter != INVALID_ENTITY_INDEX) {
                resourceProducer->particleConstructor(scene, sprite->pos, sprite->nation, accepter);
                if (unit->type == UnitType_TIMBERLAND) {
                    terrain->timber[(int)(sprite->pos.x / 64) * 2 + (int)(sprite->pos.y / 64) * 2 * 2 * terrain->tileSize] *= 0.999f;
                    terrain->timber[(int)(sprite->pos.x / 64) * 2 + 1 + (int)(sprite->pos.y / 64) * 2 * 2 * terrain->tileSize] *= 0.999f;
                    terrain->timber[(int)(sprite->pos.x / 64) * 2 + ((int)(sprite->pos.y / 64) * 2 + 1) * 2 * terrain->tileSize] *= 0.999f;
                    terrain->timber[(int)(sprite->pos.x / 64) * 2 + 1 + ((int)(sprite->pos.y / 64) * 2 + 1) * 2 * terrain->tileSize] *= 0.999f;

                    resourceProducer->resourceTicksTotal = 1.0f * ticksPerLabor / Terrain_GetTimber(terrain, (int)sprite->pos.x, (int)sprite->pos.y);
                } else if (unit->type == UnitType_MINE) {
                    terrain->ore[(int)(sprite->pos.x / 64) * 2 + (int)(sprite->pos.y / 64) * 2 * 2 * terrain->tileSize] *= 0.999f;
                    terrain->ore[(int)(sprite->pos.x / 64) * 2 + 1 + (int)(sprite->pos.y / 64) * 2 * 2 * terrain->tileSize] *= 0.999f;
                    terrain->ore[(int)(sprite->pos.x / 64) * 2 + ((int)(sprite->pos.y / 64) * 2 + 1) * 2 * terrain->tileSize] *= 0.999f;
                    terrain->ore[(int)(sprite->pos.x / 64) * 2 + 1 + ((int)(sprite->pos.y / 64) * 2 + 1) * 2 * terrain->tileSize] *= 0.999f;

                    resourceProducer->resourceTicksTotal = 1.0f * ticksPerLabor / (resourceProducer->type == ResourceType_ORE ? Terrain_GetOre(terrain, (int)sprite->pos.x, (int)sprite->pos.y) : (2 * Terrain_GetCoal(terrain, (int)sprite->pos.x, (int)sprite->pos.y)));
                }

                if (unit->type == UnitType_FOUNDRY || unit->type == UnitType_POWERPLANT) { // Intentional no else-if
                    ResourceAccepter* resourceAccepter = (ResourceAccepter*)Scene_GetComponent(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID);
                    resourceProducer->resourceTicksRemaining = -1; // This is reset whenever a foundry receives particles from mines
                    resourceAccepter->storage[ResourceType_ORE] = 0; // Ok to do for powerplant
                    resourceAccepter->storage[ResourceType_COAL] = 0;
                } else {
                    resourceProducer->resourceTicksRemaining = resourceProducer->resourceTicksTotal;
                }
            }
        } else if (resourceProducer->resourceTicksRemaining > 0) {
            if (unit->type == UnitType_FOUNDRY) {
                ResourceAccepter* resourceAccepter = (ResourceAccepter*)Scene_GetComponent(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID);
                if (resourceAccepter->ticksSinceLastAccept[ResourceType_POWER] < 3 * ticksPerLabor) {
                    resourceProducer->resourceTicksRemaining--;
                    resourceAccepter->storage[ResourceType_POWER] = 1; // no need to take power if not producing anything
                } else {
                    resourceAccepter->storage[ResourceType_POWER] = 0; // Signify that you need power
                }
            } else {
                resourceProducer->resourceTicksRemaining--;
            }
        } // Don't decrement if < 0

        // If building and no capital, kys
        if ((unit->type == UnitType_MINE || unit->type == UnitType_TIMBERLAND) && sprite->nation->capital == INVALID_ENTITY_INDEX) {
            unit->isDead = true;
        }
    }
}

/*
	For every producer, decrements time left for producer. If there are no ticks 
	left, produces the unit. 
	
	If the producer's "repeat" flag is set, repeats the order a second time. */
void Match_ProduceUnits(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, PRODUCER_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);
        City* city = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
        ResourceAccepter* resourceAccepter = (ResourceAccepter*)Scene_GetComponent(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID);

        bool hasFood = sprite->nation->resources[ResourceType_POPULATION] - (sprite->nation->unitCount[UnitType_FARM] + sprite->nation->unitCount[UnitType_CITY]) < 5 * sprite->nation->unitCount[UnitType_FARM];
        if (producer->order != -1 && hasFood) {
            if (resourceAccepter->ticksSinceLastAccept[ResourceType_POWER] < 3 * ticksPerLabor) {
                producer->orderTicksRemaining--;
            } else {
                resourceAccepter->storage[ResourceType_POWER] = 0;
            }
        } else {
            if (hasFood) {
                producer->orderTicksRemaining--;
            }
            resourceAccepter->storage[ResourceType_POWER] = 1; // no need to take power if not producing anything
        }

        if (producer->order != -1 && producer->orderTicksRemaining == 0) {
            sprite->nation->prodCount[producer->order]--;
            sprite->nation->unitCount[producer->order]++;
            sprite->nation->resources[ResourceType_POPULATION]++;

            if (producer->order == UnitType_INFANTRY) {
                Infantry_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_ENGINEER) {
                Engineer_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_CAVALRY) {
                Cavalry_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_ARTILLERY) {
                Artillery_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_TRANSPORT) {
                Transport_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_DESTROYER) {
                Destroyer_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_CRUISER) {
                Cruiser_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_BATTLESHIP) {
                Battleship_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_FIGHTER) {
                Fighter_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_ATTACKER) {
                Attacker_Create(scene, sprite->pos, sprite->nation);
            } else if (producer->order == UnitType_BOMBER) {
                Bomber_Create(scene, sprite->pos, sprite->nation);
            } else {
                PANIC("Producer's can't build that UnitType: %d", producer->order);
            }

            if (sprite->nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                char buffer[32];
                memset(buffer, 0, 32);
                Match_CopyUnitName(unit->type, buffer);
                Match_AddMessage(activeColor, "Order completed at %s %s", city->name, buffer);
            }

            if (!producer->repeat || !Match_PlaceOrder(scene, sprite->nation, producer, expansion, producer->order)) {
                if (producer->repeat && sprite->nation->controlFlag == PLAYER_FLAG_COMPONENT_ID) {
                    char buffer[32];
                    memset(buffer, 0, 32);
                    Match_CopyUnitName(unit->type, buffer);
                    Match_AddMessage(errorColor, "Insufficient resources at %s %s, auto re-order canceled", city->name, buffer);
                }
                producer->order = -1;
                producer->repeat = false;
                unit->guiContainer = producer->readyGUIContainer;
                guiChange = true;
            }
        }
    }
}

void Match_UpdateResourceAccepterTicks(Scene* scene)
{
    system(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID)
    {
        ResourceAccepter* accepter = (ResourceAccepter*)Scene_GetComponent(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID);

        for (int i = 0; i < _ResourceType_Length; i++) {
            if (accepter->storage[i] + accepter->transit[i] < accepter->capacity[i]) {
                accepter->ticksSinceLastAccept[i]++;
            }
        }
    }
}

void Match_UpdateMessageContainer()
{
    for (int i = 0; i < messages->size; i++) {
        struct message* message = (struct message*)Arraylist_Get(messages, i);
        message->fade++;
        if (message->fade >= 360) {
            Arraylist_Remove(messages, i);
            i--;
        }
    }
}

/*
	Goes through every expansion, sets the nation of the 
	producer to be the nation of the homeCity */
void Match_UpdateExpansionAllegiance(struct scene* scene)
{
    system(scene, id, EXPANSION_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = sprite->nation;
        if (!unit->isDead && Scene_EntityHasComponents(scene, expansion->homeCity, SPRITE_COMPONENT_ID)) {
            Sprite* homeCitySprite = (Sprite*)Scene_GetComponent(scene, expansion->homeCity, SPRITE_COMPONENT_ID);
            if (sprite->nation != homeCitySprite->nation) {
                Nation* newNation = homeCitySprite->nation;
                Match_SwapAllegiance(scene, nation, newNation, id);
            }
        }
    }
}

/*
	Takes in a scene, iterates through all entities with Sprite and 
	Transform components. Translates texture based on Terrain's offset and zoom,
	colorizes based on the nation color, and renders texture to screen. */
void Match_SpriteRender(struct scene* scene, ComponentKey layer)
{
    system(scene, id, SPRITE_COMPONENT_ID, layer)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        SDL_Rect rect = { 0, 0, 0, 0 };
        if (sprite->hidden) {
            continue;
        }

        int shadowZ = (int)(sprite->z < 0.5 ? 2 : 60 * sprite->z - 28);
        int deathTicks = !Scene_EntityHasComponents(scene, id, UNIT_COMPONENT_ID) ? 16 : 16 - ((Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID))->deathTicks;

        // Shadow
        SDL_SetTextureAlphaMod(sprite->shadow, 255.0f / 16.0f * deathTicks);
        Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y, (float)sprite->width, (float)sprite->height);
        Texture_Draw(sprite->shadow, rect.x, rect.y, (float)rect.w, (float)rect.h, sprite->angle);

        // Outline
        if (deathTicks == 16 && sprite->spriteOutline) {
            if (sprite->showOutline) {
                SDL_SetTextureAlphaMod(sprite->spriteOutline, 255);
                Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y - shadowZ, (float)sprite->width, (float)sprite->height);
                Texture_Draw(sprite->spriteOutline, rect.x, rect.y, (float)rect.w, (float)rect.h, sprite->angle);
            } else if (sprite->hitTicks > 0) {
                SDL_SetTextureAlphaMod(sprite->spriteOutline, (Uint8)(sprite->hitTicks / 18.0f * 255));
                Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y - shadowZ, (float)sprite->width, (float)sprite->height);
                Texture_Draw(sprite->spriteOutline, rect.x, rect.y, (float)rect.w, (float)rect.h, sprite->angle);
            }
        }

        // Base image
        Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y - shadowZ, (float)sprite->width, (float)sprite->height);
        if (!sprite->destroyOnBounds) {
            SDL_Color nationColor = sprite->nation->color;
            SDL_SetTextureColorMod(sprite->sprite, nationColor.r, nationColor.g, nationColor.b);
        } else {
            SDL_SetTextureColorMod(sprite->sprite, 255, 255, 255);
        }
        SDL_SetTextureAlphaMod(sprite->sprite, 255.0f / 16.0f * deathTicks);
        Texture_Draw(sprite->sprite, rect.x, rect.y, (float)rect.w, (float)rect.h, sprite->angle);

        // Reset alpha mods
        SDL_SetTextureAlphaMod(sprite->shadow, 255.0f);
        SDL_SetTextureAlphaMod(sprite->spriteOutline, 255.0f);
        SDL_SetTextureAlphaMod(sprite->sprite, 255.0f);
    }
}

void Match_RenderResourceIndicators(Scene* scene)
{
    SDL_Rect rect = { 0, 0, 0, 0 };
    for (int x = 16; x < terrain->size; x += 32) {
        float cos = fastCos(((int)(Apricot_Ticks + x) % 628) / 100.0);
        for (int y = 16; y < terrain->size; y += 32) {
            int x0 = x / 32;
            int y0 = y / 32;

            float height = Terrain_GetHeight(terrain, x, y);
            if (height <= 0.5f) {
                continue;
            }

            float timber = terrain->timber[x0 + y0 * terrain->tileSize * 2];
            float coal = -terrain->ore[x0 + y0 * terrain->tileSize * 2];
            float ore = terrain->ore[x0 + y0 * terrain->tileSize * 2];

            if (timber > 0.5 && timber >= coal && timber >= ore) {
                Terrain_Translate(&rect, x, y - (height - 0.5f) * 16.0f, 16, 40);
                Texture_Draw(TIMBER_INDICATOR_TEXTURE_ID, rect.x, rect.y, rect.w, rect.h, cos * height * 0.2f);
            } else if (coal > 0.5 && coal > timber && coal >= ore) {
                Terrain_Translate(&rect, x, y - (height - 0.5f) * 16.0f, 16, 16);
                Texture_Draw(COAL_INDICATOR_TEXTURE_ID, rect.x, rect.y, rect.w, rect.h, 0);
            } else if (ore > 0.5 && ore > coal && ore > timber) {
                Terrain_Translate(&rect, x, y - (height - 0.5f) * 16.0f, 16, 16);
                Texture_Draw(ORE_INDICATOR_TEXTURE_ID, rect.x, rect.y, rect.w, rect.h, 0);
            }
        }
    }
}

double getArrowRects(Vector from, Vector to, float z, Vector centerOfMass, float* scale, SDL_Rect* rect, SDL_Rect* dest, SDL_Rect* arrow)
{
    *scale = min(1, Vector_Dist(from, to) / 64.0f * Terrain_GetZoom());
    Vector newFrom = from;
    Terrain_Translate(rect, newFrom.x, newFrom.y, 0, 0);
    if (Apricot_Keys[SDL_SCANCODE_LSHIFT]) { // Offset by center of mass, calculated earlier
        Vector distToCenter = Vector_Sub(from, centerOfMass);
        to = Vector_Add(to, distToCenter);
    }
    to = Terrain_LineOfSightPoint(terrain, from, to, z);
    double angle = Vector_Angle(Vector_Sub(from, to));
    Terrain_Translate(arrow, to.x, to.y, 0, 0);
    // Walk back some pixels
    to = Vector_Add(to, Vector_Scalar(Vector_Normalize(Vector_Sub(from, to)), 31.0f / Terrain_GetZoom() * (*scale)));
    Terrain_Translate(dest, to.x, to.y, 0, 0);
    return angle;
}

void Match_DrawSelectionArrows(Scene* scene)
{
    Vector centerOfMass = { 0, 0 };
    // If shift is held down, find center of mass of selected units
    if (Apricot_Keys[SDL_SCANCODE_LSHIFT]) {
        int numSelected = 0;
        system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, TARGET_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
        {
            Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
            Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
            if (target->selected) {
                centerOfMass = Vector_Add(centerOfMass, sprite->pos);
                numSelected++;
            }
        }
        if (numSelected != 0) {
            centerOfMass = Vector_Scalar(centerOfMass, 1.0f / numSelected);
        }
    }
    // Draw all the shadows (so that one shadow of an arrow isnt on top of another)
    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        SDL_Rect rect = { 0, 0, 0, 0 };
        SDL_Rect dest = { 0, 0, 0, 0 };
        SDL_Rect arrow = { 0, 0, 0, 0 };

        Vector to = sprite->pos;
        Vector from = sprite->pos;
        if (target->selected) {
            if (Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID)) {
                Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
                from = patrol->patrolPoint;
            } else {
                from = sprite->pos;
            }
            to = Terrain_MousePos();
        } else if (unit->isHovered) {
            if (Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID)) {
                Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
                to = patrol->patrolPoint;
            } else {
                to = target->tar;
            }
        }

        if (Vector_Dist(sprite->pos, to) > 1) {
            float scale = 1.0;
            double angle = getArrowRects(from, to, sprite->z, centerOfMass, &scale, &rect, &dest, &arrow);
            Texture_Draw(ARROW_SHADOW_TEXTURE_ID, arrow.x - 32 * scale, arrow.y - 32 * scale + 2, 64 * scale, 64 * scale, angle);
            Texture_DrawThickLine(NULL, (Vector) { dest.x, dest.y + 2 }, (Vector) { rect.x, rect.y + 2 }, (SDL_Color) { 0, 0, 0, 64 }, 8 * scale);
        }
    }
    // Draw the arrows
    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        SDL_Rect rect = { 0, 0, 0, 0 };
        SDL_Rect dest = { 0, 0, 0, 0 };
        SDL_Rect arrow = { 0, 0, 0, 0 };

        Vector to = sprite->pos;
        Vector from = sprite->pos;
        if (target->selected) {
            if (Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID)) {
                Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
                from = patrol->patrolPoint;
            } else {
                from = sprite->pos;
            }
            to = Terrain_MousePos();
        } else if (unit->isHovered) {
            if (Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID)) {
                Patrol* patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
                to = patrol->patrolPoint;
            } else {
                to = target->tar;
            }
        }

        if (Vector_Dist(sprite->pos, to) > 1) {
            float scale = 1.0;
            double angle = getArrowRects(from, to, sprite->z, centerOfMass, &scale, &rect, &dest, &arrow);
            int shadowZ = (int)(sprite->z < 0.5 ? 2 : 60 * sprite->z - 28);
            Texture_DrawThickLine(NULL, (Vector) { dest.x, dest.y - shadowZ }, (Vector) { rect.x, rect.y - shadowZ }, (SDL_Color) { 0, 79, 206, 180 }, 8 * scale);
            SDL_SetTextureAlphaMod(ARROW_TEXTURE_ID, 180);
            Texture_Draw(ARROW_TEXTURE_ID, arrow.x - 32 * scale, arrow.y - 32 * scale - shadowZ, 64 * scale, 64 * scale, angle);
        }
    }
}

void Match_UpdateGUIElements(struct scene* scene)
{
    if (currShownEntity != INVALID_ENTITY_INDEX) {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, currShownEntity, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, currShownEntity, UNIT_COMPONENT_ID);

        char buffer[32];
        memset(buffer, 0, 32);
        Match_CopyUnitName(unit->type, buffer);
        if (Scene_EntityHasComponents(scene, currShownEntity, CITY_COMPONENT_ID)) {
            City* city = (City*)Scene_GetComponent(scene, currShownEntity, CITY_COMPONENT_ID);
            GUI_SetLabelText(scene, unitNameLabel, "%s", city->name);
        } else if (Scene_EntityHasComponents(scene, currShownEntity, EXPANSION_COMPONENT_ID)) {
            Expansion* expansion = (Expansion*)Scene_GetComponent(scene, currShownEntity, EXPANSION_COMPONENT_ID);
            City* homeCity = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
            GUI_SetLabelText(scene, unitNameLabel, "%s %s", homeCity->name, buffer);
        } else {
            char suffixBuffer[3];
            memset(suffixBuffer, 0, 3);
            Match_GetOrdinalSuffix(unit->ordinal, suffixBuffer);
            GUI_SetLabelText(scene, unitNameLabel, "%d%s %s", unit->ordinal, suffixBuffer, buffer);
        }

        ProgressBar* unitBar = (ProgressBar*)Scene_GetComponent(scene, unitHealthBar, GUI_PROGRESS_BAR_COMPONENT_ID);
        unitBar->value = unit->health / 100.0f;

        if (unit->focused && Scene_EntityHasComponents(scene, currShownEntity, PRODUCER_COMPONENT_ID)) {
            Producer* producer = (Producer*)Scene_GetComponent(scene, currShownEntity, PRODUCER_COMPONENT_ID);
            char orderBuffer[32] = "Order: ";
            Match_CopyUnitName(producer->order, orderBuffer);
            GUI_SetLabelText(scene, orderLabel, orderBuffer);

            if (producer->orderTicksRemaining > 0) {
                int seconds = producer->orderTicksRemaining / 60;
                int minutes = seconds / 60;
                GUI_SetLabelText(scene, timeLabel, "Time remaining: %dm %ds", minutes, (seconds - minutes * 60));
            } else {
                GUI_SetLabelText(scene, timeLabel, "");
            }
            GUI_SetRockerSwitchValue(scene, autoReOrderRockerSwitch, producer->repeat);
        }

        GUIComponent* boardButton = (GUIComponent*)Scene_GetComponent(scene, boardButtonID, GUI_COMPONENT_ID);
        GUIComponent* cancelBoardButton = (GUIComponent*)Scene_GetComponent(scene, cancelBoardButtonID, GUI_COMPONENT_ID);
        boardButton->shown = !boardMode;
        cancelBoardButton->shown = boardMode;

        // Check the building tile for the currently shown entity, if it's an ID for a port, set active to true
        EntityID buildingTile = Terrain_GetBuildingAt(terrain, sprite->pos.x, sprite->pos.y);
        if (!boardMode && buildingTile != INVALID_ENTITY_INDEX) {
            Unit* buildingUnit = (Unit*)Scene_GetComponent(scene, buildingTile, UNIT_COMPONENT_ID);
            boardButton->active = buildingUnit->type == UnitType_PORT;
        } else {
            boardButton->active = false;
        }
    }
}

void Match_DrawVisitedSquares(Scene* scene)
{
    for (int i = 0; i < nations->size; i++) {
        Nation* nation = (Nation*)Arraylist_Get(nations, i);
        if (nation->capital == INVALID_ENTITY_INDEX) {
            continue;
        }
        SDL_Rect rect = { 0, 0, 0, 0 };
        for (int x = 0; x < nation->visitedSpacesSize; x++) {
            for (int y = 0; y < nation->visitedSpacesSize; y++) {
                float urgency = nation->visitedSpaces[x + y * nation->visitedSpacesSize];
                if (urgency < 0 && Apricot_Ticks % 60 < 30) {
                    Terrain_Translate(&rect, x * 32.0f, y * 32.0f, 32, 32);
                    SDL_SetRenderDrawColor(Apricot_Renderer, nation->color.r, nation->color.g, nation->color.b, 150);
                    SDL_RenderFillRect(Apricot_Renderer, &rect);
                } else if (urgency == 0) {
                    Terrain_Translate(&rect, x * 32.0f, y * 32.0f, 32, 32);
                    SDL_SetRenderDrawColor(Apricot_Renderer, nation->color.r, nation->color.g, nation->color.b, 50);
                    SDL_RenderFillRect(Apricot_Renderer, &rect);
                }
            }
        }
    }
}

void Match_DrawPortTiles(Scene* scene)
{
    for (int i = 0; i < terrain->ports->size; i++) {
        Vector tile = *(Vector*)Arraylist_Get(terrain->ports, i);
        SDL_Rect rect = { 0, 0, 0, 0 };
        Terrain_Translate(&rect, tile.x, tile.y, 64, 64);
        SDL_SetRenderDrawColor(Apricot_Renderer, 255, 0, 0, 150);
        SDL_RenderFillRect(Apricot_Renderer, &rect);
    }
}

void Match_DrawBoxSelect(Scene* scene)
{
    if (boxTL.x != -1) {
        SDL_Rect rect = { (int)boxTL.x, (int)boxTL.y, (int)boxBR.x - (int)boxTL.x, (int)boxBR.y - (int)boxTL.y };
        Terrain_Translate(&rect, boxTL.x + (boxBR.x - boxTL.x) / 2, boxTL.y + (boxBR.y - boxTL.y) / 2, boxBR.x - boxTL.x, boxBR.y - boxTL.y);
        SDL_SetRenderDrawColor(Apricot_Renderer, 0, 79, 206, 50);
        SDL_RenderFillRect(Apricot_Renderer, &rect);
    }
}

void Match_UpdateFogOfWar(struct scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, AI_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = sprite->nation;

        unit->engagedTicks--;
        sprite->hidden = nation->controlFlag != PLAYER_FLAG_COMPONENT_ID && unit->engagedTicks < 0;
    }
}

void Match_DrawMiniMap(Scene* scene)
{
    SDL_Rect rect = { 0, Apricot_Height - miniMapSize - 2, miniMapSize + 2, miniMapSize + 2 };
    SDL_SetRenderDrawColor(Apricot_Renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(Apricot_Renderer, &rect);
    rect = (SDL_Rect) { 0, Apricot_Height - miniMapSize, miniMapSize, miniMapSize };
    SDL_RenderCopy(Apricot_Renderer, miniMapTexture, NULL, &rect);

    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        if (sprite->hidden) {
            continue;
        }
        SDL_SetRenderDrawColor(Apricot_Renderer, nation->color.r, nation->color.g, nation->color.b, 255);
        rect = (SDL_Rect) { sprite->pos.x * miniMapSize / (terrain->tileSize * 64) - 3, Apricot_Height - miniMapSize + sprite->pos.y * miniMapSize / (terrain->tileSize * 64) - 3, 6, 6 };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
    }
}

void Match_RenderOrderButtons(Scene* scene)
{
    Nation* nation = Arraylist_Get(nations, 0);
    system(scene, id, ORDER_BUTTON_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }
        Clickable* clickable = (Clickable*)Scene_GetComponent(scene, id, GUI_CLICKABLE_COMPONENT_ID);
        OrderButton* orderButton = (OrderButton*)Scene_GetComponent(scene, id, ORDER_BUTTON_COMPONENT_ID);
        gui->active = Match_CheckResources(nation, orderButton->type) == -1;

        // Draw background, active -> regular, inactive -> lighter
        if (gui->active) {
            SDL_SetRenderDrawColor(Apricot_Renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        } else {
            SDL_SetRenderDrawColor(Apricot_Renderer, inactiveBackgroundColor.r, inactiveBackgroundColor.g, inactiveBackgroundColor.b, inactiveBackgroundColor.a);
        }
        SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };
        SDL_RenderFillRect(Apricot_Renderer, &rect);
        SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);

        // hovered & active -> lighten background, draw border
        if (gui->isHovered && gui->active) {
            SDL_SetRenderDrawColor(Apricot_Renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_RenderFillRect(Apricot_Renderer, &rect);
            SDL_SetRenderDrawColor(Apricot_Renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        }

        // Draw button icon
        SDL_SetTextureColorMod(orderButton->icon, activeColor.r, activeColor.g, activeColor.b);
        Texture_DrawCentered(orderButton->icon, gui->pos.x + 8, gui->pos.y + 8, 32, 32, 0);

        // Draw button text
        if (gui->active) {
            FC_SetDefaultColor(font, (SDL_Color) { 255, 255, 255, 255 });
        } else {
            FC_SetDefaultColor(font, (SDL_Color) { inactiveTextColor.r, inactiveTextColor.g, inactiveTextColor.b, inactiveTextColor.a });
        }
        FC_Draw(font, Apricot_Renderer, gui->pos.x + 48, gui->pos.y + 2, clickable->text);

        int width = 0;
        for (ResourceType i = 0; i < _ResourceType_Length; i++) {
            if (nation->costs[i][orderButton->type] == 0) {
                continue;
            }
            if (nation->costs[i][orderButton->type] <= nation->resources[i]) {
                FC_SetDefaultColor(font, (SDL_Color) { 255, 255, 255, 255 });
            } else {
                FC_SetDefaultColor(font, (SDL_Color) { errorColor.r, errorColor.g, errorColor.b, errorColor.a });
            }
            FC_Draw(font, Apricot_Renderer, gui->pos.x + 48 + width, gui->pos.y + 22, "%d", nation->costs[i][orderButton->type]);
            width += FC_GetWidth(font, "%d", nation->costs[i][orderButton->type]) + 1;
            Texture_Draw(Match_LookupResourceTypeIcon(i), gui->pos.x + 48 + width, gui->pos.y + 27, 15, 15, 0);
            width += 15 + 15;
        }
    }
    FC_SetDefaultColor(font, (SDL_Color) { 255, 255, 255, 255 });
}

void Match_RenderUnitLists(Scene* scene)
{
    if (!Scene_EntityHasComponents(scene, currShownEntity, BOARDABLE_COMPONENT_ID)) {
        return;
    }

    bool shown = false;
    system(scene, id, UNIT_LIST_COMPONENT_ID, GUI_COMPONENT_ID)
    {
        GUIComponent* gui = (GUIComponent*)Scene_GetComponent(scene, id, GUI_COMPONENT_ID);
        if (!gui->shown) {
            continue;
        }

        Unit* unit = (Unit*)Scene_GetComponent(scene, currShownEntity, UNIT_COMPONENT_ID);
        Boardable* boardable = (Boardable*)Scene_GetComponent(scene, currShownEntity, BOARDABLE_COMPONENT_ID);
        if (unit->focused) {
            SDL_Rect rect = { gui->pos.x, gui->pos.y, gui->width, gui->height };

            FC_SetDefaultColor(font, (SDL_Color) { 255, 255, 255, 255 });
            FC_Draw(font, Apricot_Renderer, gui->pos.x, gui->pos.y, "%d", boardable->size);
            shown = true;
            break;
        }

        if (shown) {
            break;
        }
    }
}

void Match_RenderNationInfo(Scene* scene)
{
    SDL_Rect rect = { 0, 0, 140, 102 };
    SDL_SetRenderDrawColor(Apricot_Renderer, 21, 21, 21, 180);
    SDL_RenderFillRect(Apricot_Renderer, &rect);

    Nation* nation = Arraylist_Get(nations, 0);

    Texture_Draw(COIN_TEXTURE_ID, 8, 8, 20, 20, 0);
    FC_Draw(font, Apricot_Renderer, 36, 6, "%d", nation->resources[ResourceType_COIN]);
    Texture_Draw(FOOD_TEXTURE_ID, 8, 32, 20, 20, 0);
    FC_Draw(font, Apricot_Renderer, 36, 30, "%d / %d", nation->resources[ResourceType_POPULATION] - (nation->unitCount[UnitType_FARM] + nation->unitCount[UnitType_CITY]), 5 * nation->unitCount[UnitType_FARM]);
    Texture_Draw(TIMBER_TEXTURE_ID, 8, 56, 20, 20, 0);
    FC_Draw(font, Apricot_Renderer, 36, 54, "%d", nation->resources[ResourceType_TIMBER]);
    Texture_Draw(METAL_TEXTURE_ID, 8, 80, 20, 20, 0);
    FC_Draw(font, Apricot_Renderer, 36, 78, "%d", nation->resources[ResourceType_METAL]);
}

void Match_RenderCityName(Scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, CITY_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        City* city = (City*)Scene_GetComponent(scene, id, CITY_COMPONENT_ID);

        if (sprite->hidden) {
            continue;
        }
        SDL_Rect rect;
        FC_Scale scale;
        if (city->isCapital) {
            scale = (FC_Scale) { (Terrain_GetZoom() + 0.5f) * 0.08f, (Terrain_GetZoom() + 0.5f) * 0.08f };
        } else {
            // TODO: Change back to +0.3f scale
            scale = (FC_Scale) { (Terrain_GetZoom() + 0.5f) * 0.08f, (Terrain_GetZoom() + 0.5f) * 0.08f };
        }
        int width = FC_GetWidth(bigFont, city->name) * scale.x;
        int height = FC_GetAscent(bigFont, city->name) * scale.y;
        Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y + 16, 0, 0);
        FC_SetDefaultColor(bigFont, (SDL_Color) { 0, 0, 0, 255 });
        FC_DrawScale(bigFont, Apricot_Renderer, rect.x - width / 2 + 1, rect.y - height / 2 + 1, scale, city->name);
        FC_SetDefaultColor(bigFont, (SDL_Color) { 255, 255, 255, 255 });
        FC_DrawScale(bigFont, Apricot_Renderer, rect.x - width / 2, rect.y - height / 2, scale, city->name);
    }
}

void Match_RenderMessageContainer(Scene* scene)
{
    int heightOffset = 0;
    int focusGUIY = ((GUIComponent*)Scene_GetComponent(scene, focusedGUIContainer, GUI_COMPONENT_ID))->pos.y;
    for (int i = messages->size - 1; i >= 0; i--) {
        struct message* message = (struct message*)Arraylist_Get(messages, i);
        float fade = message->fade < 300 ? 1.0f : (360.0f - message->fade) / 60.0f;
        SDL_Rect box = { -252, -252, 252, 1000 };

        // Draw string off screen in order to get height of message
        box = FC_DrawBox(font, Apricot_Renderer, box, message->text);
        box.h += 6;
        heightOffset += box.h;
        // Reset x and y to normal values
        box.x = Apricot_Width - 252;
        box.y = focusGUIY - heightOffset - 2;

        // Draw background
        SDL_SetRenderDrawColor(Apricot_Renderer, 21, 21, 21, 180 * fade);
        SDL_RenderFillRect(Apricot_Renderer, &box);

        // Draw shadow
        box.x += 9;
        box.y += 3;
        FC_SetDefaultColor(font, (SDL_Color) { 21, 21, 21, 255 * fade });
        FC_DrawBox(font, Apricot_Renderer, box, message->text);

        // Draw text
        box.y -= 1;
        box.x -= 1;
        FC_SetDefaultColor(font, (SDL_Color) { message->color.r, message->color.g, message->color.b, 255 * fade });
        FC_DrawBox(font, Apricot_Renderer, box, message->text);
    }
    // Reset font to default color before exiting
    FC_SetDefaultColor(font, (SDL_Color) { 255, 255, 255, 255 });
}

static void Match_RenderBar(Vector pos, int remaining, int total, SDL_Color color)
{
    // Get the rectangle for full bar
    SDL_Rect rect;
    Terrain_Translate(&rect, pos.x, pos.y - 13, 22, 6);

    // Draw the background bar
    SDL_SetRenderDrawColor(Apricot_Renderer, 21, 21, 21, 180);
    SDL_RenderFillRect(Apricot_Renderer, &rect);

    // Draw inner bar, slightly inset
    rect.x += Terrain_GetZoom() * 2;
    rect.y += Terrain_GetZoom() * 2;
    rect.w = Terrain_GetZoom() * 18 * (1.0f - (float)remaining / (float)total);
    rect.h -= Terrain_GetZoom() * 4 - 1;
    SDL_SetRenderDrawColor(Apricot_Renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(Apricot_Renderer, &rect);
}

void Match_RenderProducerBars(Scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, PRODUCER_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (producer->orderTicksRemaining > 0) {
            Match_RenderBar(sprite->pos, producer->orderTicksRemaining, producer->orderTicksTotal, sprite->nation->color);
        }
    }
    system(scene, id, SPRITE_COMPONENT_ID, RESOURCE_PRODUCER_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        ResourceProducer* producer = (ResourceProducer*)Scene_GetComponent(scene, id, RESOURCE_PRODUCER_COMPONENT_ID);

        if (producer->type == ResourceType_METAL && producer->resourceTicksRemaining >= 0) {
            Match_RenderBar(sprite->pos, producer->resourceTicksRemaining, producer->resourceTicksTotal, sprite->nation->color);
        }
    }
}

void Match_NoPowerSymbols(Scene* scene)
{
    system(scene, id, SPRITE_COMPONENT_ID, RESOURCE_ACCEPTER_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        ResourceAccepter* accepter = (ResourceAccepter*)Scene_GetComponent(scene, id, RESOURCE_ACCEPTER_COMPONENT_ID);

        SDL_Rect rect = { 0, 0, 0, 0 };

        if (accepter->ticksSinceLastAccept[ResourceType_POWER] % 60 > 30) {
            Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y, 24, 24);
            Texture_Draw(NO_POWER_WARNING_TEXTURE_ID, rect.x, rect.y, (float)rect.w, (float)rect.h, 0);

        } else if (accepter->ticksSinceLastAccept[ResourceType_COAL] % 60 > 30) {
            Terrain_Translate(&rect, sprite->pos.x, sprite->pos.y, 24, 24);
            Texture_Draw(NO_COAL_WARNING_TEXTURE_ID, rect.x, rect.y, (float)rect.w, (float)rect.h, 0);
        }
    }
}

/*
	Runs each update system, every tick */
void Match_Update(Scene* match)
{
    Terrain_Update(terrain);

    Match_AIUpdateVisitedSpaces(match);
    Match_SetVisitedSpace(match);

    Match_Unit(match);

    Match_Hover(match);
    Match_Select(match);
    Match_Focus(match);

    Match_Patrol(match);
    Match_Target(match);
    Match_SpriteMove(match);
    Match_ResourceParticle(match);
    Match_BombMove(match);
    Match_CombatantAttack(match);
    Match_AirplaneAttack(match);
    Match_AirplaneScout(match);

    Match_UpdateResourceAccepterTicks(match);
    Match_ProduceResources(match);
    Match_ProduceUnits(match);
    Match_UpdateExpansionAllegiance(match);

    Match_AI(match);
#ifdef _DEBUG
    Terrain_ClosestMaskDist(match, CITY_COMPONENT_ID, terrain, 0, 0); // If an error was thrown here, means a building didn't update the building map when it died.
#endif

    Match_UpdateMessageContainer(match);
    if (doFogOfWar) { // Determined by a switch in the match creation menu
        Match_UpdateFogOfWar(match);
    }
    Match_UpdateGUIElements(match);
    GUI_Update(match);

    static bool lt = false;
    static bool gt = false;
    // Change game tick speed
    if (!lt && Apricot_Keys[SDL_SCANCODE_COMMA]) {
        Apricot_DeltaT *= 2.0;
        Match_AddMessage(textColor, "Time warp: %.01fx", 16.0f / Apricot_DeltaT);
        ignoreMissedTicks = true;
    } else if (!gt && Apricot_Keys[SDL_SCANCODE_PERIOD]) {
        Apricot_DeltaT *= 0.5;
        Match_AddMessage(textColor, "Time warp: %.01fx", 16.0f / Apricot_DeltaT);
    }
    lt = Apricot_Keys[SDL_SCANCODE_COMMA];
    gt = Apricot_Keys[SDL_SCANCODE_PERIOD];

    // Placed at end because they pop game scene
    Match_CheckWin(match);
    Match_EscapePressed(match);
}

/*
	Runs each render system, every screen draw */
void Match_Render(Scene* match)
{
    Terrain_Render(terrain);
    Match_RenderResourceIndicators(match);

    Match_SpriteRender(match, BUILDING_LAYER_COMPONENT_ID);
    Match_NoPowerSymbols(match);
    Match_RenderProducerBars(match);
    Match_RenderCityName(match);

    Match_DrawSelectionArrows(match);

    Match_SpriteRender(match, SURFACE_LAYER_COMPONENT_ID);
    Match_SpriteRender(match, AIR_LAYER_COMPONENT_ID);
    Match_SpriteRender(match, PLANE_LAYER_COMPONENT_ID);
    Match_SpriteRender(match, PARTICLE_LAYER_COMPONENT_ID);

    //Match_DrawVisitedSquares(match);
    //Match_DrawPortTiles(match);

    Match_DrawBoxSelect(match);
    Match_DrawMiniMap(match);
    Match_RenderMessageContainer(match);
    GUI_Render(match);
    Match_RenderOrderButtons(match);
    Match_RenderUnitLists(match);
    Match_RenderNationInfo(match);
}

void Match_EngineerAddCity(Scene* scene, EntityID guiID)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        if (unit->focused) {
            Match_BuyCity(scene, sprite->nation, sprite->pos);
        }
    }
}

void Match_EngineerAddExpansion(Scene* scene, EntityID guiID)
{
    UnitType type = (UnitType)((Clickable*)Scene_GetComponent(scene, guiID, GUI_CLICKABLE_COMPONENT_ID))->meta;
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        if (unit->focused) {
            Match_BuyExpansion(scene, type, sprite->nation, sprite->pos);
        }
    }
}

void Match_EngineerAddBuilding(Scene* scene, EntityID guiID)
{
    UnitType type = (UnitType)((Clickable*)Scene_GetComponent(scene, guiID, GUI_CLICKABLE_COMPONENT_ID))->meta;
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        if (unit->focused) {
            Match_BuyBuilding(scene, type, sprite->nation, sprite->pos);
        }
    }
}

void Match_EngineerAddWall(Scene* scene, EntityID guiID)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        if (unit->focused && nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_WALL]) {
            Match_BuyWall(scene, nation, sprite->pos, sprite->angle);
        }
    }
}

void Match_ProducerOrder(Scene* scene, EntityID buttonID)
{
    // Buttons have the type stored in their meta data
    UnitType type = (UnitType)((Clickable*)Scene_GetComponent(scene, buttonID, GUI_CLICKABLE_COMPONENT_ID))->meta;
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);

        if (unit->focused && Match_PlaceOrder(scene, nation, producer, expansion, type)) {
            unit->guiContainer = producer->busyGUIContainer;
            guiChange = true;
            break;
        }
    }
}

void Match_BoardUnit(Scene* scene, EntityID buttonID)
{
    boardMode = true;
    system(scene, id, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID)
    {
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        if (unit->focused) {
            boardUnitID = id;
            target->selected = false;
            break;
        }
    }
}

void Match_CancelBoardUnit(Scene* scene, EntityID buttonID)
{
    boardMode = false;
    boardUnitID = INVALID_ENTITY_INDEX;
}

void Match_DestroyUnit(Scene* scene, EntityID buttonID)
{
    system(scene, id, UNIT_COMPONENT_ID)
    {
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        if (unit->focused) {
            unit->isDead = true;
            break;
        }
    }
}

void Match_ProducerCancelOrder(Scene* scene, EntityID guiID)
{
    system(scene, id, SPRITE_COMPONENT_ID, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (unit->focused) {
            producer->orderTicksRemaining = -10;
            producer->order = -1;
            producer->repeat = false;
            unit->guiContainer = producer->readyGUIContainer;
            guiChange = true;
        }
    }
}

void Match_ProducerReOrder(Scene* scene, EntityID rockerID)
{
    RockerSwitch* rockerSwitch = (RockerSwitch*)Scene_GetComponent(scene, rockerID, GUI_ROCKER_SWITCH_COMPONENT_ID);
    system(scene, id, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID)
    {
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (unit->focused) {
            producer->repeat = rockerSwitch->value;
        }
    }
}

void Match_Destroy(Scene* scene)
{
    GUI_Destroy(scene);
    Terrain_Destroy(terrain);
    SDL_DestroyTexture(miniMapTexture);
    Arraylist_Destroy(messages);
    for (int i = 0; i < nations->size; i++) {
        Nation* nation = Arraylist_Get(nations, i);
        free(nation->visitedSpaces);
        Arraylist_Destroy(nation->cities);
    }
    Arraylist_Destroy(nations);
}

Scene* Match_Init(Terrain* _terrain, char* capitalName, Lexicon* lexicon, bool AIControlled, bool fogOfWar, int nNations)
{
    terrain = _terrain;
    Scene* match = Scene_Create(&Components_Register, &Match_Update, &Match_Render, &Match_Destroy);
    miniMapTexture = SDL_CreateTexture(Apricot_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, miniMapSize, miniMapSize);
    Texture_PaintMap(terrain->map, terrain->size, miniMapTexture, Terrain_MiniMapColor);
    nations = Arraylist_Create(1, sizeof(Nation));
    messages = Arraylist_Create(10, sizeof(struct message));
    GUI_Register(match);
    doFogOfWar = fogOfWar;
    printf("Match: %p\n", match);

    orderLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    timeLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    autoReOrderRockerSwitch = GUI_CreateRockerSwitch(match, (Vector) { 100, 100 }, "Auto Re-order", false, &Match_ProducerReOrder);
    boardButtonID = GUI_CreateButton(match, (Vector) { 0, 0 }, 204, 48, "Board", 0, &Match_BoardUnit);
    cancelBoardButtonID = GUI_CreateButton(match, (Vector) { 0, 0 }, 204, 48, "Cancel Boarding", 0, &Match_CancelBoardUnit);

    unitNameLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "Lol!");
    unitHealthBar = GUI_CreateProgressBar(match, (Vector) { 0, 0 }, 168, 1.0f);

    focusedGUIContainer = GUI_CreateContainer(match, (Vector) { 251, 389 }, 140, 202);
    GUI_SetBackgroundColor(match, focusedGUIContainer, (SDL_Color) { 19, 20, 23, 180 });
    GUI_SetBorder(match, focusedGUIContainer, 2);
    GUI_SetPadding(match, focusedGUIContainer, 8);
    GUI_SetMargin(match, focusedGUIContainer, -5);
    GUI_ContainerAdd(match, focusedGUIContainer, unitNameLabel);
    GUI_ContainerAdd(match, focusedGUIContainer, unitHealthBar);
    GUI_ContainerAdd(match, focusedGUIContainer, orderLabel);
    GUI_ContainerAdd(match, focusedGUIContainer, autoReOrderRockerSwitch);
    GUI_ContainerAdd(match, focusedGUIContainer, timeLabel);
    GUI_ContainerAdd(match, focusedGUIContainer, GUI_CreateSpacer(match, (Vector) { 0, 0 }, 0, 203));
    GUI_ContainerAdd(match, focusedGUIContainer, GUI_CreateLabel(match, (Vector) { 0, 0 }, "Actions"));

    ENGINEER_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 251, 389 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, ENGINEER_FOCUSED_GUI);
    GUI_SetPadding(match, ENGINEER_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Farm", FARM_TEXTURE_ID, UnitType_FARM, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Timberland", TIMBERLAND_TEXTURE_ID, UnitType_TIMBERLAND, &Match_EngineerAddBuilding));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build City", CITY_TEXTURE_ID, UnitType_CITY, &Match_EngineerAddCity));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Mine", MINE_TEXTURE_ID, UnitType_MINE, &Match_EngineerAddBuilding));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Power Plant", POWERPLANT_TEXTURE_ID, UnitType_POWERPLANT, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Foundry", FOUNDRY_TEXTURE_ID, UnitType_FOUNDRY, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Academy", ACADEMY_TEXTURE_ID, UnitType_ACADEMY, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Factory", FACTORY_TEXTURE_ID, UnitType_FACTORY, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Port", PORT_TEXTURE_ID, UnitType_PORT, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Airfield", AIRFIELD_TEXTURE_ID, UnitType_AIRFIELD, &Match_EngineerAddExpansion));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, OrderButton_Create(match, "Build Wall", WALL_TEXTURE_ID, UnitType_WALL, &Match_EngineerAddWall));
    //GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, GUI_CreateSpacer(match, (Vector) { 0, 0 }, 204, 48));
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, boardButtonID);
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, cancelBoardButtonID);
    GUI_ContainerAdd(match, ENGINEER_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 0, 0 }, 204, 48, "Destroy", 0, &Match_DestroyUnit));
    GUI_SetShown(match, ENGINEER_FOCUSED_GUI, false);

    CITY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, CITY_FOCUSED_GUI);
    GUI_SetPadding(match, CITY_FOCUSED_GUI, 2);
    GUI_SetShown(match, CITY_FOCUSED_GUI, false);

    UNIT_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, UNIT_FOCUSED_GUI);
    GUI_SetPadding(match, UNIT_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, UNIT_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Destroy", 0, &Match_DestroyUnit));
    GUI_SetShown(match, UNIT_FOCUSED_GUI, false);

    TRANSPORT_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, TRANSPORT_FOCUSED_GUI);
    GUI_SetPadding(match, TRANSPORT_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, TRANSPORT_FOCUSED_GUI, UnitList_Create(match));
    GUI_SetShown(match, TRANSPORT_FOCUSED_GUI, false);

    ACADEMY_READY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, ACADEMY_READY_FOCUSED_GUI);
    GUI_SetPadding(match, ACADEMY_READY_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, ACADEMY_READY_FOCUSED_GUI, OrderButton_Create(match, "Recruit Infantry", INFANTRY_TEXTURE_ID, UnitType_INFANTRY, &Match_ProducerOrder));
    GUI_ContainerAdd(match, ACADEMY_READY_FOCUSED_GUI, OrderButton_Create(match, "Recruit Engineer", ENGINEER_TEXTURE_ID, UnitType_ENGINEER, &Match_ProducerOrder));
    GUI_ContainerAdd(match, ACADEMY_READY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Destroy", 0, &Match_DestroyUnit));
    GUI_SetShown(match, ACADEMY_READY_FOCUSED_GUI, false);

    ACADEMY_BUSY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, ACADEMY_BUSY_FOCUSED_GUI);
    GUI_SetPadding(match, ACADEMY_BUSY_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, ACADEMY_BUSY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Cancel Order", 0, &Match_ProducerCancelOrder));
    GUI_SetShown(match, ACADEMY_BUSY_FOCUSED_GUI, false);

    FACTORY_READY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, FACTORY_READY_FOCUSED_GUI);
    GUI_SetPadding(match, FACTORY_READY_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Cavalry", CAVALRY_TEXTURE_ID, UnitType_CAVALRY, &Match_ProducerOrder));
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Artillery", ARTILLERY_TEXTURE_ID, UnitType_ARTILLERY, &Match_ProducerOrder));
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Fighter", FIGHTER_TEXTURE_ID, UnitType_FIGHTER, &Match_ProducerOrder));
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Attacker", ATTACKER_TEXTURE_ID, UnitType_ATTACKER, &Match_ProducerOrder));
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Bomber", BOMBER_TEXTURE_ID, UnitType_BOMBER, &Match_ProducerOrder));
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Destroy", 0, &Match_DestroyUnit));
    GUI_SetShown(match, FACTORY_READY_FOCUSED_GUI, false);

    FACTORY_BUSY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, FACTORY_BUSY_FOCUSED_GUI);
    GUI_SetPadding(match, FACTORY_BUSY_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, FACTORY_BUSY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Cancel Order", 0, &Match_ProducerCancelOrder));
    GUI_SetShown(match, FACTORY_BUSY_FOCUSED_GUI, false);

    PORT_READY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, PORT_READY_FOCUSED_GUI);
    GUI_SetPadding(match, PORT_READY_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, PORT_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Transport", DESTROYER_TEXTURE_ID, UnitType_TRANSPORT, &Match_ProducerOrder));
    GUI_ContainerAdd(match, PORT_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Destroyer", DESTROYER_TEXTURE_ID, UnitType_DESTROYER, &Match_ProducerOrder));
    GUI_ContainerAdd(match, PORT_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Cruiser", CRUISER_TEXTURE_ID, UnitType_CRUISER, &Match_ProducerOrder));
    GUI_ContainerAdd(match, PORT_READY_FOCUSED_GUI, OrderButton_Create(match, "Build Battleship", BATTLESHIP_TEXTURE_ID, UnitType_BATTLESHIP, &Match_ProducerOrder));
    GUI_ContainerAdd(match, PORT_READY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Destroy", 0, &Match_DestroyUnit));
    GUI_SetShown(match, PORT_READY_FOCUSED_GUI, false);

    PORT_BUSY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 0, 0 }, 140, 190);
    GUI_ContainerAdd(match, focusedGUIContainer, PORT_BUSY_FOCUSED_GUI);
    GUI_SetPadding(match, PORT_BUSY_FOCUSED_GUI, 2);
    GUI_ContainerAdd(match, PORT_BUSY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 204, 48, "Cancel Order", 0, &Match_ProducerCancelOrder));
    GUI_SetShown(match, PORT_BUSY_FOCUSED_GUI, false);

    int tileSize = terrain->size / 64;
    int trySize = 10000;
    Arraylist* nationVectors = Arraylist_Create(nNations, sizeof(Vector));
    Arraylist* tempVectors = Arraylist_Create(nNations, sizeof(Vector));
    for (int j = 0; j < nNations; j++) {
        Vector v = { 0, 0 };
        Arraylist_Add(&tempVectors, &v);
    }
    double largestDist = 0;
    // Try a couple different positions
    for (int i = 0; i < trySize; i++) {
        // Try random tiles
        for (int j = 0; j < nNations; j++) {
            float randX = 64 * (rand() % tileSize) + 32;
            float randY = 64 * (rand() % tileSize) + 32;
            Vector v = { randX, randY };
            if (!Terrain_IsSolidSquare(terrain, v) || (Terrain_GetTimber(terrain, randX, randY) > 0.5 && Terrain_GetOre(terrain, randX, randY) > 0.5 && Terrain_GetCoal(terrain, randX, randY) > 0.5)) {
                j--;
                continue;
            }
            Arraylist_Put(tempVectors, j, &v);
        }

        // Find the product of all the distances in the graph of nations. Remember if it is larger
        double tempDist = 1;
        for (int j = 0; j < nNations - 1; j++) {
            for (int k = j + 1; k < nNations; k++) {
                float dist = Vector_Dist(*(Vector*)Arraylist_Get(tempVectors, j), *(Vector*)Arraylist_Get(tempVectors, k));
                if (dist < 7 * 64) {
                    tempDist = 0;
                    j = nNations;
                    break;
                } else {
                    tempDist += dist;
                }
            }
        }
        if (tempDist > largestDist) {
            Arraylist_Copy(&nationVectors, tempVectors);
            largestDist = tempDist;
        }
    }

    if (largestDist > 0) {
        Terrain_SetOffset((Vector) { terrain->size / 2, terrain->size / 2 });
        SDL_Color nationColors[] = {
            (SDL_Color) { 0, 79, 206 }, // 1 Blue
            (SDL_Color) { 202, 20, 21 }, // 2 Red
            (SDL_Color) { 97, 191, 34 }, // 3 Green
            (SDL_Color) { 255, 129, 18 }, // 4 Orange
            (SDL_Color) { 255, 0, 255 }, // 5 Magenta
            (SDL_Color) { 0, 192, 155 }, // 6 Turqoise
            (SDL_Color) { 247, 216, 1 }, // 7 Yellow
            (SDL_Color) { 109, 0, 205 }, // 8 Purple
            (SDL_Color) { 125, 236, 227 }, // 9 Cyan
            (SDL_Color) { 255, 153, 255 }, // 10 fuschia
            (SDL_Color) { 121, 224, 119 }, // 11 lime
            (SDL_Color) { 249, 249, 249 }, // 12 white
        };
        // Add nations first. When lists are resized, their pointers change.
        for (int i = 0; i < sizeof(nationColors) / sizeof(SDL_Color); i++) {
            Nation temp;
            memset(&temp, 0, sizeof(Nation));
            Arraylist_Add(&nations, &temp); // Copy temp over, will be garbage until it's init'd
        }
        // Init nations, with the first nation being player controlled
        for (int i = 0; i < sizeof(nationColors) / sizeof(SDL_Color); i++) {
            Nation* nation = (Nation*)Arraylist_Get(nations, i); // This is the real nation ptr that should be passed around
            EntityID capital;
            Vector nationVector;
            if (i < nNations) {
                nationVector = *(Vector*)Arraylist_Get(nationVectors, i);
            } else {
                nationVector = (Vector) { 0, 0 };
            }
            // Init nation in place
            if (i == 0 && !AIControlled) {
                Nation_Create(match, nation, NULL, nationColors[i], terrain->size, PLAYER_FLAG_COMPONENT_ID);
                Terrain_SetOffset(nationVector);
                capital = City_Create(match, nationVector, nation, capitalName, true);
            } else {
                char enemyNameBuffer[20];
                Lexicon_GenerateWord(lexicon, enemyNameBuffer, 15);
                Nation_Create(match, nation, AI_Init, nationColors[i], terrain->size, AI_COMPONENT_ID);
                if (i < nNations) {
                    capital = City_Create(match, nationVector, nation, enemyNameBuffer, true);
                }
            }
            if (i < nNations) {
                nation->capitalPos = nationVector;
                // Register capital
                Terrain_SetBuildingAt(terrain, capital, (int)nationVector.x, (int)nationVector.y);
                Nation_SetCapital(match, nation, capital);

                // Create engineers
                nation->unitCount[UnitType_ENGINEER]++; // Done first, so that engineer unit ordinal is correct
                Engineer_Create(match, nationVector, nation);
            }
        }

        // Make all nations enemies of each other, find their key continents
        for (int i = 0; i < sizeof(nationColors) / sizeof(SDL_Color) - 1; i++) {
            for (int j = i + 1; j < sizeof(nationColors) / sizeof(SDL_Color); j++) {
                Nation* nation1 = (Nation*)Arraylist_Get(nations, i);
                Nation* nation2 = (Nation*)Arraylist_Get(nations, j);
                Arraylist_Add(&(nation1->enemyNations), &nation2);
                Arraylist_Add(&(nation2->enemyNations), &nation1);
            }
        }
        Terrain_EliminateUselessPortPoints(terrain, nationVectors);
    } else {
        Match_AddMessage(errorColor, "Space for capitals could not be found!");
    }

    ignoreMissedTicks = true;
    Apricot_PushScene(match);

    return match;
}