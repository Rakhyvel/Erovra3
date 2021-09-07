#ifndef GOAP_H
#define GOAP_H

#include "./scene.h"
#include <stdbool.h>

#define MAX_VARIABLES 64
#define MAX_ACTIONS 64

typedef Uint8 VariableID;
typedef Uint8 ActionID;

/* 
* Actions have preconditions, a function pointer to a function, and a post-
* condition that should be true after the action runs.
* 
* An action's children in a graph is all actions in the graph whose effect is 
* the first false precondition in the action's array of preconditions.
* 
* Actions have costs associated with them, these can be updated in real time,
* and the Goap engine will choose the plan that has the lowest cost.
*/
typedef struct action {
    VariableID preconditions[8]; // Variables that action sees as preconditions before running, in order
    int numPreconditions;
    void (*action)(Scene* scene, ComponentKey flag); // Action to complete
    int cost; // Cost of this action
} Action;

typedef struct goap {
    // Maps variables to a list of actions that cause the variable to become true
    Arraylist* effects[MAX_VARIABLES];

    // "Variables" should be enums
    bool variables[MAX_VARIABLES];
    void (*updateVariableSystem)(Scene* scene, struct goap* goap, ComponentKey flag); // Updates the variables according to the scene state

    // Actions are allocated sequentially
    Action actions[MAX_ACTIONS];
    int numActions;
} Goap;

/* Creates a Goal-Oriented Action Planning struct that is used for AIs. If 
* there are more one, separate AIs in a scene, they each should have a separate
* GOAP, because the variables are likely specific to themselves. 
* 
* @param goapInit	A function that is responsible for setting the 
*					variableUpdateSystem, and add all the actions
* @return The allocated goap, with all actions already added/
*/
Goap* Goap_Create(void (*goapInit)(Goap* goap));

/* Adds an action to a GOAP struct.
* 
* @param goap				The GOAP of the AI to add the action to
* @param action				The function pointer for the actual action of the 
*							action
* @param cost				The cost of doing the action
* @param effect				The variable that the action sets to true after 
*							performed
* @param numPreconditions	The number of preconditions the action has
* @param preconditions		Varargs of the variable IDs that are used for 
*							preconditions
*/
void Goap_AddAction(Goap* goap, void (*action)(Scene* scene, ComponentKey flag), int cost, VariableID effect, int numPreconditions, VariableID preconditions, ...);

/*
* Performs Dijkstra's algorithm on graph, finds the action (system) to execute,
* and executes it.
* 
* @param scene	The scene that the AI inhabits
* @param goap	The GOAP struct to use for AI
* @param flag	The flag used to distinguish entities as part of this AI
*/
void Goap_Update(Scene* scene, Goap* goap, ComponentKey flag);

#endif