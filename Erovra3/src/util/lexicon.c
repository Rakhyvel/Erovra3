#pragma once
#include "lexicon.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	Swaps two edges data, but preserves the linked list pointers for simplicity. */
void swapEdges(struct lexicon_edge* a, struct lexicon_edge* b)
{
    Lexicon* tempTo = a->to;
    int tempWeight = a->weight;

    a->to = b->to;
    a->weight = b->weight;

    b->to = tempTo;
    b->weight = tempWeight;
}

int particleCompare(char* a, char* b)
{
    for (int i = 0; i < PARTICLE_SIZE; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

void addEdge(Lexicon* from, Lexicon* to)
{
    struct lexicon_edge* curr = from->edges;
    // Search for edge in list
    while (curr != NULL) {
        // Edge already in list, update weights and skip
        if (particleCompare(curr->to->text, to->text)) {
            curr->weight++;
            while (curr->next != NULL && curr->next->weight < curr->weight) {
                swapEdges(curr, curr->next);
                curr = curr->next;
            }
            from->numConnections++;
            break;
        }
        curr = curr->next;
    }
    // Edge was not found in list, create new edge
    if (curr == NULL) {
        struct lexicon_edge* edge = calloc(1, sizeof(struct lexicon_edge));
        if (!edge) {
            PANIC("Mem error...");
        }
        edge->weight = 1;
        edge->next = from->edges;
        edge->to = to;
        from->numConnections++;
        from->edges = edge;
    }
}

Lexicon* addNode(Lexicon* graph, char* buffer)
{
    Lexicon* newNode = calloc(1, sizeof(Lexicon));
    if (!newNode) {
        PANIC("Mem error...");
    }
    for (int i = 0; i < PARTICLE_SIZE; i++) {
        newNode->text[i] = buffer[i];
    }
    // Add to end
    Lexicon* curr = graph;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = newNode;

    return newNode;
}

/*
	Create a Lexicon graph
	Pull 3 characters from file for the graph
	Pull another character from file, concat onto last 2 characters 
	*/
Lexicon* Lexicon_Create(char* filename)
{
    FILE* file;
	// Adjacency list head
    Lexicon* graph;
	// Used for comparing text read in from file
    char buffer[PARTICLE_SIZE + 1];
	// Current visited node in adjacency list
    Lexicon* workingNode;

    fopen_s(&file, filename, "r");
    if (!file) {
        perror(filename);
        PANIC("Crashing please wait...");
    }
    graph = calloc(1, sizeof(Lexicon));
    if (!graph) {
        PANIC("Mem error...");
    }

    memset(buffer, 0, PARTICLE_SIZE + 1);
    for (int i = 0; i < PARTICLE_SIZE; i++) {
        graph->text[i] = fgetc(file);
        buffer[i] = graph->text[i];
    }

    workingNode = graph;
    do {
        // Left shift buffer bytes
        for (int i = 0; i < PARTICLE_SIZE; i++) {
            buffer[i] = buffer[i + 1];
        }
        // Read in next character, place in new spot
        int nextChar = fgetc(file);
        if (nextChar == EOF) {
            break;
        }
        buffer[PARTICLE_SIZE - 1] = nextChar;
        // buffer now has text of next particle node

        Lexicon* curr = graph;
        // Check to see if the particle is already in graph
        while (curr != NULL) {
            if (particleCompare(curr->text, buffer)) {
                // Create an edge from workingNode to new
                addEdge(workingNode, curr);
                workingNode = curr;
                break;
            }
            curr = curr->next;
        }
        if (curr == NULL) {
            Lexicon* newNode = addNode(graph, buffer);
            addEdge(workingNode, newNode);
            workingNode = newNode;
        }
    } while (1);

    fclose(file);
    return graph;
}

/*
	Generates a random word based on a Lexicon.
	
	Chooses a random node in the graph to start off at. Then, chooses a random edge
	in the graph, representing choosing a preceding letter. Higher weighted edges
	are chosen more frequently. */
void Lexicon_GenerateWord(Lexicon* lex, char* out, int maxLength)
{
    char buffer[PARTICLE_SIZE + 1];
    memset(buffer, 0, PARTICLE_SIZE + 1);
    int outIndex = 0;
    // Set buffer[0-size) to first text of lexicon
    for (int i = 0; i < PARTICLE_SIZE; i++, outIndex++) {
        buffer[i] = lex->text[i];
        out[outIndex] = buffer[i];
    }
    while (outIndex < maxLength) {
        // Search through lexicon list
        Lexicon* curr = lex;
        while (curr != NULL) {
            // Look for particles that match the buffer
            if (particleCompare(curr->text, buffer) && curr->edges != NULL) {
                // Match found, choose a random edge based on weights
                struct lexicon_edge* edge = curr->edges;
                int randProb = rand() % curr->numConnections;
                while (edge != NULL) {
                    randProb -= edge->weight;
                    if (randProb <= 0) {
                        break;
                    }
                    edge = edge->next;
                }
				// Reset buffer, add character to out string, and set curr to the new node
                Lexicon* next = edge->to;
                for (int i = 0; i < PARTICLE_SIZE; i++) {
                    buffer[i] = next->text[i];
                }
                out[outIndex++] = buffer[PARTICLE_SIZE - 1];
                curr = next;
                break;
            }
            curr = curr->next;
        }
        if (curr == NULL) {
            break;
        }
    }
}

/*
	Prints out the adjacency list representing the graph */
void Lexicon_PrintGraph(Lexicon* graph)
{
    Lexicon* curr = graph;
    while (curr != NULL) {
        printf("%s : ", curr->text);
        struct lexicon_edge* edge = curr->edges;
        while (edge != NULL) {
            printf("%s(%d), ", edge->to->text, edge->weight);
            edge = edge->next;
        }
        printf("\n");
        curr = curr->next;
    }
}