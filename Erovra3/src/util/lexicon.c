#include "lexicon.h"
#include "debug.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
	Swaps two edges data, but preserves the linked list pointers for simplicity. */
static void swapEdges(struct lexicon_edge* a, struct lexicon_edge* b)
{
    Lexicon* tempTo = a->to;
    int tempWeight = a->weight;

    a->to = b->to;
    a->weight = b->weight;

    b->to = tempTo;
    b->weight = tempWeight;
}

/* Returns 1 if the strings are exactly the same */
static int particleCompare(char* a, char* b)
{
    for (int i = 0; i < PARTICLE_SIZE; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

static void createEdge(Lexicon* from, Lexicon* to, int weight)
{
    struct lexicon_edge* edge = calloc(1, sizeof(struct lexicon_edge));
    if (!edge) {
        PANIC("Mem error...");
    }
    edge->weight = weight;
    edge->next = from->edges;
    edge->to = to;
    from->numConnections += weight;
    from->edges = edge;
    struct lexicon_edge* curr = from->edges;
    while (curr->next != NULL && curr->next->weight < curr->weight) {
        swapEdges(curr, curr->next);
        curr = curr->next;
    }
}

static void addEdge(Lexicon* from, Lexicon* to)
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
        createEdge(from, to, 1);
    }
}

static Lexicon* addNode(Lexicon* graph, char* buffer)
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
void Lexicon_Create(char* filename, char* outFilename, int* status)
{
    FILE* file;
    // Adjacency list head
    Lexicon* graph;
    // Used for comparing text read in from file
    char buffer[PARTICLE_SIZE + 1];
    memset(buffer, 0, PARTICLE_SIZE + 1);
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

    for (int i = 0; i < PARTICLE_SIZE; i++) {
        graph->text[i] = fgetc(file);
        buffer[i] = graph->text[i];
    }

    workingNode = graph;
    int k = 0;
    do {
        // Left shift buffer bytes
        for (int i = 0; i < PARTICLE_SIZE; i++) {
            buffer[i] = buffer[i + 1];
        }
        // Read in next character, place in new spot
        int nextChar = fgetc(file);
        if (nextChar == EOF) {
            break;
        } else if (nextChar == '\n' || nextChar == ':' || nextChar == ';') {
            nextChar = '_';
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
        // Particle was not in graph
        if (curr == NULL) {
            Lexicon* newNode = addNode(graph, buffer);
            addEdge(workingNode, newNode);
            workingNode = newNode;
            (*status)++;
        }
        k++;
    } while (k < 80000);

    fclose(file);
    FILE* out;
    fopen_s(&out, outFilename, "w");
    Lexicon_PrintGraph(graph, out);
    fclose(out);
}

Lexicon* Lexicon_Read(char* filename)
{
    FILE* file;
    fopen_s(&file, filename, "r");
    if (!file) {
        perror(filename);
        PANIC("Crashing, please wait...");
    }

    Lexicon* graph = NULL;

    char line[256];
    char* token;
    char* context = NULL;
    while (fgets(line, sizeof(line), file)) {
        token = strtok_s(line, ":", &context);
        Lexicon* newNode = NULL;
        if (graph == NULL) {
            graph = calloc(1, sizeof(Lexicon));
            for (int i = 0; i < PARTICLE_SIZE; i++) {
                graph->text[i] = token[i];
            }
            newNode = graph;
        } else {
            // See if the node is in the list already
            newNode = graph;
            while (newNode != NULL) {
                if (particleCompare(newNode->text, token)) {
                    break;
                }
                newNode = newNode->next;
            }
            // Node was not in graph already, create it
            if (newNode == NULL) {
                newNode = addNode(graph, token);
            }
        }

        char* edgeTo = NULL;
        char* weight = NULL;
        while ((edgeTo = strtok_s(NULL, ";", &context)) != NULL && (weight = strtok_s(NULL, ";", &context)) != NULL) {
            // Check to see if node is in graph already
            Lexicon* curr = graph;
            while (curr != NULL) {
                if (particleCompare(curr->text, edgeTo)) {
                    // Node is already in adj list. Create edge with specified weight
                    createEdge(newNode, curr, atoi(weight));
                    break;
                }
                curr = curr->next;
            }
            // Particle was not in graph, create it, create edge with specified weight
            if (curr == NULL) {
                Lexicon* toNode = addNode(graph, edgeTo);
                createEdge(newNode, toNode, atoi(weight));
            }
        }
    }
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
    bool found = false;
    Lexicon* original = lex;
    int length;
    Lexicon* curr = original;
    for (length = 0; curr != NULL; curr = curr->next, length++)
        ;
    while (!found) {
        memset(out, 0, (size_t)maxLength + 1);
        lex = original;

        // Choose a random particle that starts with a capital letter
        int randIndex = rand() % length;
        for (int i = 0; lex->next != NULL; lex = lex->next, i++) {
            if (i > randIndex && lex->text[0] >= 'A' && lex->text[0] <= 'Z') {
                break;
            }
        }
        char buffer[PARTICLE_SIZE + 1];
        memset(buffer, 0, PARTICLE_SIZE + 1);
        int outIndex = 0;
        // Set buffer[0-size) to first text of lexicon
        for (int i = 0; i < PARTICLE_SIZE; i++, outIndex++) {
            buffer[i] = lex->text[i];
            out[outIndex] = buffer[i];
        }
        // Stay within string length
        curr = lex;
        while (outIndex < maxLength - 1) {
            // Search through lexicon list
            while (curr != NULL) {
                // Look for particles that match the buffer
                if (particleCompare(curr->text, buffer) && curr->edges != NULL) {
                    // Match found, choose a random edge based on weights
                    struct lexicon_edge* edge = curr->edges;
                    int randProb = rand() % curr->numConnections;
                    while (edge != NULL) {
                        randProb -= edge->weight;
                        if (randProb <= 1) {
                            break;
                        }
                        edge = edge->next;
                    }
                    // Reset buffer, add character to out string, and set curr to the new node
                    Lexicon* next = edge->to;
                    for (int i = 0; i < PARTICLE_SIZE; i++) {
                        buffer[i] = next->text[i];
                    }

                    // Determine if should keep going or not
                    if (next->text[PARTICLE_SIZE - 1] == '_') {
                        curr = NULL; // End word generation
                    } else {
                        out[outIndex++] = buffer[PARTICLE_SIZE - 1];
                        curr = next; // Transfer over to new particle
                    }
                    break;
                }
                curr = curr->next;
            }
            if (curr == NULL) {
                found = true;
                break;
            }
        }
    }
}

/*
	Prints out the adjacency list representing the graph */
void Lexicon_PrintGraph(Lexicon* graph, FILE* out)
{
    Lexicon* curr = graph;
    while (curr != NULL) {
        fprintf(out, "%s:", curr->text);
        struct lexicon_edge* edge = curr->edges;
        while (edge != NULL) {
            fprintf(out, "%s;%d;", edge->to->text, edge->weight);
            edge = edge->next;
        }
        fprintf(out, "\n");
        curr = curr->next;
    }
}