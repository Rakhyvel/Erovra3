#include "./goap.h"
#include <string.h>

Goap* Goap_Create(void (*goapInit)(Goap* goap))
{
    Goap* goap = calloc(1, sizeof(Goap));
    for (int i = 0; i < MAX_VARIABLES; i++) {
        goap->effects[i] = Arraylist_Create(10, sizeof(Action));
    }
    goapInit(goap);
    return goap;
}
/* 
* 1. Creates action
* 2. Adds to actions array
* 3. Increments numActions, becomes actionID
* 4. Adds to list mapped to by effect variableID
*		Create list if need to
*/
void Goap_AddAction(Goap* goap, void (*actionPtr)(Scene* scene, ComponentKey flag), int cost, VariableID effect, int numPrecoditions, VariableID preconditions, ...)
{
    Action action;
    action.action = actionPtr;
    action.cost = cost;
    action.numPreconditions = numPrecoditions;

    va_list args;
    va_start(args, preconditions);
    for (int i = 0; i < numPrecoditions; i++) {
        action.preconditions[i] = va_arg(args, VariableID);
    }
    va_end(args);

    // Add action to effects list corresponding to its effect
    Arraylist_Add(goap->effects[effect], &action);
    // Add action to array of actions
    goap->actions[goap->numActions++] = action;
}

void Goap_Update(Scene* scene, Goap* goap, ComponentKey flag)
{

}
