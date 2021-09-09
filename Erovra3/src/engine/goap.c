#include "./goap.h"
#include "../util/debug.h"
#include <string.h>

Goap* Goap_Create(void (*goapInit)(Goap* goap))
{
    Goap* goap = calloc(1, sizeof(Goap));
    if (!goap) {
        PANIC("Mem error");
    }
    for (int i = 0; i < MAX_VARIABLES; i++) {
        goap->effects[i] = Arraylist_Create(10, sizeof(ActionID));
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
    action.actionPtr = actionPtr;
    action.cost = cost;
    action.numPreconditions = numPrecoditions;

    va_list args;
    va_start(args, preconditions);
    action.preconditions[0] = preconditions;
    for (int i = 1; i < numPrecoditions; i++) {
        action.preconditions[i] = va_arg(args, VariableID);
    }
    va_end(args);

    // Add action to effects list corresponding to its effect
    Arraylist_Add(goap->effects[effect], &(goap->numActions));
    // Add action to array of actions
    goap->actions[goap->numActions++] = action;
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
// TODO: Use a priority queue (maybe)
static ActionID minDistance(int dist[], bool processed[])
{
    // Initialize min value
    ActionID min = INT_MAX, min_index;

    for (int v = 0; v < MAX_ACTIONS; v++)
        if (!processed[v] && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

void Goap_Update(Scene* scene, Goap* goap, ComponentKey flag)
{
    // Update the variables before running planner
    goap->updateVariableSystem(scene, goap, flag);

    int dist[MAX_ACTIONS]; // The cost from each action the to the main goal
    bool processed[MAX_ACTIONS]; // Whether or not the action has been processed
    bool isLeaf[MAX_ACTIONS]; // Whether or not the action is good

    for (int i = 0; i < MAX_ACTIONS; i++) {
        dist[i] = INT_MAX;
        processed[i] = false;
        isLeaf[i] = true;
    }
    dist[0] = 0;

    // Loop through all actions
    for (int count = 0; count < MAX_ACTIONS - 1; count++) {
        ActionID u = minDistance(dist, processed);
        processed[u] = true;

        Action action = goap->actions[u];
        // For each false pre-condition in action:
        for (int i = 0; i < action.numPreconditions; i++) {
            if (!goap->variables[action.preconditions[i]]) {
                isLeaf[u] = false;

                // For each action that makes this false pre-condition true:
                Arraylist* children = goap->effects[action.preconditions[i]];
                for (int j = 0; j < children->size; j++) {
                    ActionID v = ARRAYLIST_GET_DEREF(children, j, ActionID);
                    Action child = goap->actions[v];

                    // Update dist[v] only if is not processed, there is an
                    // edge from u to v, and total weight of path from src to
                    // v through u is smaller than current value of dist[v]
                    if (!processed[v] && dist[u] != INT_MAX
                        && dist[u] + child.cost < dist[v])
                        dist[v] = dist[u] + child.cost;
                }
            }
        }
    }

    // Find the action with the lowest cost that is a leaf node
    ActionID best = -1;
    int bestDist = INT_MAX;
    for (int i = 0; i < goap->numActions; i++) {
        if (dist[i] < bestDist && isLeaf[i]) {
            bestDist = dist[i];
            best = i;
        }
    }

	// Perform the best action, if there is any
    if (best != -1 && best < MAX_ACTIONS) {
        Action bestAction = goap->actions[best];
        if (bestAction.actionPtr) {
            bestAction.actionPtr(scene, flag);
        }
    }
}
