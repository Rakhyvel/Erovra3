/*	goap.c
* 
*	@author Joseph Shimel
*	@date	9/7/21
*/

#include "./goap.h"
#include "../util/debug.h"
#include "./apricot.h"
#include <string.h>

static ActionID minDistance(int dist[], bool processed[]);

void Goap_Create(Goap* goap, void (*goapInit)(Goap* goap))
{
    if (!goap) {
        PANIC("Goap is null");
    }
    for (int i = 0; i < MAX_VARIABLES; i++) {
        goap->effects[i] = NULL;
    }
    memset(goap, 0, sizeof(Goap));
    goapInit(goap);
}

void Goap_AddAction(Goap* goap, char* name, void (*actionPtr)(Scene* scene, ComponentKey flag), VariableID effect, int numPrecoditions, Uint8 preconditions, ...)
{
    if (numPrecoditions > MAX_PRECONDITIONS) {
        PANIC("Too many preconditions for GOAP action. Max is %d, received %d", MAX_PRECONDITIONS, numPrecoditions);
    }
    Action action;
    action.actionPtr = actionPtr;
    action.numPreconditions = numPrecoditions;
    memset(action.name, 0, sizeof(action.name));
    strcpy_s(action.name, sizeof(action.name), name);

    va_list args;
    va_start(args, preconditions);
    action.preconditions[0] = preconditions;
    int i;
    for (i = 1; i < numPrecoditions; i++) {
        action.preconditions[i] = va_arg(args, VariableID);
    }
    for (; i < numPrecoditions * 2; i++) {
        action.costs[i - numPrecoditions] = va_arg(args, Uint8);
    }
    va_end(args);

    // Add action to effects list corresponding to its effect
    if (goap->effects[effect] == NULL) {
        goap->effects[effect] = Arraylist_Create(3, sizeof(ActionID));
    }
    Arraylist_Add(&goap->effects[effect], &(goap->numActions));
    // Add action to array of actions
    goap->actions[goap->numActions++] = action;
}

void Goap_Update(Scene* scene, Goap* goap, void* intelligence)
{
    // Update the variables before running planner
    goap->updateVariableSystem(scene, goap, intelligence);

    int dist[MAX_ACTIONS]; // The cost from each action the to the main goal
    ActionID parent[MAX_ACTIONS];
    bool processed[MAX_ACTIONS]; // Whether or not the action has been processed
    bool isLeaf[MAX_ACTIONS]; // Whether or not the action has no false pre-conditions

    for (int i = 0; i < MAX_ACTIONS; i++) {
        dist[i] = INT_MAX;
        processed[i] = false;
        isLeaf[i] = true;
        parent[i] = 65;
    }
    dist[0] = 0;

    // Loop through all actions
    for (int count = 0; count < MAX_ACTIONS - 1; count++) {
        ActionID u = minDistance(dist, processed);
        processed[u] = true;

        Action action = goap->actions[u];

        // If action is a default action, automatically increase its cost to 1000
        if (action.numPreconditions == 0) {
            dist[u] += 1000;
        } else {
        // For each false pre-condition in action:
            for (int i = 0; i < action.numPreconditions; i++) {
                VariableID precondition = action.preconditions[i];
                Uint8 cost = action.costs[i];
                if (!goap->variables[precondition]) {
                    isLeaf[u] = false;

                    // For each action that makes this false pre-condition true:
                    Arraylist* children = goap->effects[precondition];
                    // The effects list will be null if there are no actions that have the effect
                    if (children != NULL) {
                        for (int j = 0; j < children->size; j++) {
                            ActionID v = *(ActionID*)Arraylist_Get(children, j);

                            // Update dist[v] only if is not processed, there is an
                            // edge from u to v, and total weight of path from src to
                            // v through u is smaller than current value of dist[v]
                            if (!processed[v] && dist[u] != INT_MAX
                                && dist[u] + cost < dist[v]) {
                                dist[v] = dist[u] + cost;
                                parent[v] = u;
                            }
                        }
                    }
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

    // Perform the best action, if there is anyApricot_
    if (best != -1 && best < MAX_ACTIONS) {
        Action bestAction = goap->actions[best];

        if (Apricot_Keys[SDL_SCANCODE_LSHIFT]) {
            for (int i = best; i != 65; i = parent[i]) {
                printf("%s(%d) <- ", goap->actions[i].name, dist[i]);
            }
            printf("\n");
        }

        if (bestAction.actionPtr) {
            bestAction.actionPtr(scene, intelligence);
        }
    }
}

/**
 * @brief Finds the action from the set of actions not yet included in shortest path tree with minimum distance
 * @param dist Maps actions to their current shortest distances
 * @param processed Maps actions to whether or not they've been processed by Dijkstra's algorithm
 * @return The action with the shortest distance that has not yet been processed.
*/
static ActionID minDistance(int dist[], bool processed[])
{
    ActionID min = INT_MAX, min_index;

    for (int v = 0; v < MAX_ACTIONS; v++) {
        if (!processed[v] && dist[v] <= min) {
            min = dist[v], min_index = v;
        }
    }

    return min_index;
}