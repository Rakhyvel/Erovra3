/*	goap.h
* 
*	A GOAP (Goal-Oriented Action-Planner) is a structure used by AIs to make and 
*	execute plans based on the state of their surroundings.
* 
*	# USER IMPLEMENTATION
*	It's advised to have a component for AI which contains a GOAP. Then, define a
*	source file which has:
*		1. A function for updating the variables in the GOAP
*		2. A function for each action in the GOAP
*		3. A function for initializing the graph in the GOAP
*	Then, for each AI entity, call Goap_Create() and pass in the pointer to the 
*	GOAP. Then, have a system that goes through each AI entity, and call 
*	Goap_Update().
* 
*	@author	Joseph Shimel
*	@date	9/7/21
*/

#ifndef GOAP_H
#define GOAP_H

#include "./scene.h"
#include <stdbool.h>

#define MAX_VARIABLES 64
#define MAX_ACTIONS 64
#define MAX_PRECONDITIONS 10

typedef Uint8 VariableID;
typedef Uint8 ActionID;

/**
* Actions have preconditions, a function pointer to a function, and a post-
* condition that should be true after the action runs.
* 
* An action's children in a graph is all actions in the graph whose effect is 
* the false preconditions in the action's array of preconditions.
* 
* Actions have costs associated with them, these can be updated in real time,
* and the Goap engine will choose the plan that has the lowest cost.
*/
typedef struct action {
    VariableID preconditions[MAX_PRECONDITIONS]; // Variables that action sees as preconditions before running, in order
    Uint8 costs[MAX_PRECONDITIONS]; // The costs for each precondition
    int numPreconditions;
    void (*actionPtr)(Scene* scene, void* intelligence); // Action to complete
    char name[20];
} Action;

/**
 * @brief lol
*/
typedef struct goap {
    // Maps variables to a list of ActionIDs of actions that cause the variable to become true
    Arraylist* effects[MAX_VARIABLES];

    // "Variables" should be enums
    bool variables[MAX_VARIABLES];
    void (*updateVariableSystem)(Scene* scene, struct goap* goap, void* intelligence); // Updates the variables according to the scene state

    // Actions are allocated sequentially
    Action actions[MAX_ACTIONS];
    int numActions;
} Goap;

/* Creates a Goal-Oriented Action Planning struct that is used for AIs.
* 
* @param goapInit	A function that is responsible for setting the 
*					variableUpdateSystem, and add all the actions
* @return The allocated goap, with all actions already added/
*/
void Goap_Create(Goap* goap, void (*goapInit)(Goap* goap));

/*
 Adds an action to a GOAP struct.
* 
* Each action acts as an "and" statement. The action function will run if all the 
* preconditions given are true. 
*
* Calling this function with the same function pointer, but different
* preconditions acts as an "or" statement. The action function will run if one
* of the preconditions is true.
* 
* In this way, it is possible to construct any desired behavior.
* 
* 
* @param goap				The GOAP of the AI to add the action to
* @param action				The function pointer for the actual action of the 
*							action
* @param cost				The cost of doing the action
* @param effect				The variable that the action sets to true after 
*							performed
* @param numPreconditions	The number of preconditions the action has
* @param preconditions		Varargs of the variable IDs that are used for 
*							preconditions, followed by the corresponding 
*							weights to each pre-condition. Actions without
*							preconditions will be "default", meaning they will
*							be selected last, but always run.
*/
void Goap_AddAction(Goap* goap, char* name, void (*actionPtr)(Scene* scene, ComponentKey flag), VariableID effect, int numPrecoditions, Uint8 preconditions, ...);

/*
* Performs Dijkstra's algorithm on graph, finds the action (system) to execute,
* and executes it.
* 
* @param scene			The scene that the AI inhabits
* @param goap			The GOAP struct to use for AI
* @param intelligence	The entity representing the AI
*/
void Goap_Update(Scene* scene, Goap* goap, void* intelligence);

#endif