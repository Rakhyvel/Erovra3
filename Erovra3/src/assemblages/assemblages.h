/*	assemblages.h
*	
*	Used to declare entity creation functions all in one location
* 
*	@author	Joseph Shimel
*	@date	9/19/21
*/

#pragma once
#include "../engine/goap.h"
#include "../engine/scene.h"
#include "../gui/gui.h"
#include "../util/vector.h"
#include "./components.h"

/*	Creates an academy entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos		Position to put the academy at
*	@param nation	Nation EntityID for academy
*	@param homeCity	EntityID of home city
*	@param dir		Offset direction relative to home city
*	@return EntityID of created academy
*/
EntityID Academy_Create(struct scene*, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

/*	Creates an airfield entity with components assigned
* 
*	@param scene		Pointer to the match scene
*	@param pos		Position to put the airfield at
*	@param nation		Nation EntityID for airfield
*	@param homeCity	EntityID of home city
*	@param dir		Offset direction relative to home city
*	@return EntityID of created airfield
*/
EntityID Airfield_Create(struct scene*, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

/*	Creates an artillery entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the artillery at
*	@param nation	Nation EntityID for artillery unit
*	@return EntityID of created artillery unit
*/
EntityID Artillery_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates an attacker entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the attacker at
*	@param nation	Nation EntityID for attacker
*	@return EntityID of created attacker
*/
EntityID Attacker_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a battleship entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created battleship
*/
EntityID Battleship_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a bomb entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the bomb at
*	@param tar	Position to move towards (not really used in this case)
*	@param attack	The attack damage dealt by the bomb
*	@param nation	Nation EntityID for bomb
*	@return EntityID of created bomb
*/
EntityID Bomb_Create(struct scene*, Vector pos, Vector tar, float attack, Nation* nation);

/*	Creates a bomber entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created bomber
*/
EntityID Bomber_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a bullet entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the bullet at
*	@param tar	Target position to fire towards
*	@param attack	Attack damage dealt by bullet
*	@param nation	Nation EntityID for bullet
*	@return EntityID of created bullet
*/
EntityID Bullet_Create(struct scene*, Vector pos, Vector tar, float attack, Nation* nation);

/*	Creates an air bullet entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param tar	Position to fire towards
*	@param attack Attack damage dealt by air bullet
*	@param nation	Nation EntityID for unit
*	@return EntityID of created air bullet
*/
EntityID AirBullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, Nation* nation);

/*	Creates a cavalry entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created cavalry
*/
EntityID Cavalry_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a city entity with components assigned
* 
*	@param scene		Pointer to the match scene
*	@param pos		Position to put the city at
*	@param nation		Nation EntityID for city
*	@param nameBuffer	Null terminated string, less than 20 chars, that contains 
*					the city's name
*	@param isCapital	Determines whether or not the city is a capital of a nation
*	@return EntityID of created city
*/
EntityID City_Create(struct scene*, Vector pos, Nation* nation, char* nameBuffer, bool isCapital);

EntityID Coal_Create(struct scene*, Vector pos, Nation* nation, EntityID accepter);

/*	Creates a coin entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the coin at
*	@param nation	Nation EntityID for coin
*	@return EntityID of created coin
*/
EntityID Coin_Create(struct scene*, Vector pos, Nation* nation, EntityID accepter);

/*	Creates a cruiser entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created cruiser
*/
EntityID Cruiser_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a destroyer entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created destroyer
*/
EntityID Destroyer_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates an engineer entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created engineer
*/
EntityID Engineer_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a factory entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created factory
*/
EntityID Factory_Create(struct scene*, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

/*	Creates a farm entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created farm
*/
EntityID Farm_Create(struct scene*, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

/*	Creates a fighter entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created fighter
*/
EntityID Fighter_Create(Scene* scene, Vector pos, Nation* nation);

/*	Creates a food particle
 * 
 * @param scene		Pointer to the match scene
 * @param pos		Position that the food particle starts on
 * @param nationID	Nation that the food particle belongs to
 */
EntityID Food_Create(struct scene* scene, Vector pos, Nation* nationID, EntityID accepter);

EntityID Foundry_Create(struct scene*, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

/*	Creates a infantry entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created infantry
*/
EntityID Infantry_Create(Scene* scene, Vector pos, Nation* nation);

EntityID Metal_Create(struct scene* scene, Vector pos, Nation* nation, EntityID accepter);

/*	Creates a mine entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created mine
*/
EntityID Mine_Create(struct scene*, Vector pos, Nation* nation);

/*	Creates a nation entity with components assigned
* 
*	@param scene			Pointer to the match scene
*	@param goap			Goal-oriented action planner to use. NULL if player controlled
*	@param color			Color of the nation
*	@param mapSize		Width and height of the terrain map
*	@param homeNation		Component key for entities of this nation
*	@param enemyNation	Component key for entities of the enemy nation
*	@param controlFlag	Component key for how the entities in the nation are controlled
*	@return EntityID of created nation
*/
void Nation_Create(struct scene* scene, Nation* nation, void (*goapInit)(Goap* goap), SDL_Color color, int mapSize, ComponentKey controlFlag);

void Nation_ResetResources(Scene* scene, Nation* nation);

/*	Assigns the capital for a nation
* 
*	@param scene		Pointer to the match scene
*	@param nationID	EntityID of the nation to assign capital to
*	@param capital	EntityID of a city entity that will be assigned as the capital
*/
void Nation_SetCapital(struct scene* scene, Nation* nationID, EntityID capital);

/*	Creates an order button entity, with apropriate components assigned
* 
*	@param scene		Pointer to the match scene
*	@param text		Text string to display on the button
*	@param icon		Icon for the order button to display
*	@param type		UnitType that the button should adjust costs for
*	@param onclick	Call back function for when button is clicked
*	@return EntityID of the order button
*/
EntityID OrderButton_Create(struct scene* scene, char* text, SDL_Texture* icon, UnitType type, GUICallback onclick);

/*	Creates an ore entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the ore at
*	@param nation	Nation EntityID for ore
*	@return EntityID of created ore
*/
EntityID Ore_Create(struct scene*, Vector pos, Nation* nationID, EntityID accepter);

/*	Creates a port entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created port
*/
EntityID Port_Create(struct scene*, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

EntityID Power_Create(struct scene* scene, Vector pos, Nation* nation, EntityID accepter);

EntityID PowerPlant_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir);

/*	Creates a shell entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position the shell originates from
*	@param tar	Target position to arc shell at
*	@param attack	Attack damage dealt by shell
*	@param nation	Nation EntityID for shell
*	@return EntityID of created shell
*/
EntityID Shell_Create(struct scene*, Vector pos, Vector tar, float attack, Nation* nation);

EntityID Timber_Create(struct scene* scene, Vector pos, Nation* nation, EntityID accepter);

EntityID Timberland_Create(struct scene* scene, Vector pos, Nation* nation);

EntityID Transport_Create(struct scene* scene, Vector pos, Nation* nation);

EntityID UnitList_Create(struct scene* scene);

/*	Creates a wall entity with components assigned
* 
*	@param scene	Pointer to the match scene
*	@param pos	Position to put the unit at
*	@param nation	Nation EntityID for unit
*	@return EntityID of created wall
*/
EntityID Wall_Create(Scene* scene, Vector pos, float angle, Nation* nation);