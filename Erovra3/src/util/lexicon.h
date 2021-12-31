#ifndef LEXICON_H
#define LEXICON_H

#include <stdio.h>
#include <stdbool.h>

#define PARTICLE_SIZE 3

/*

A graph, with nodes holding "particles" of speech, parsed from a text document.
Edges are weighted with probability of another particle following the node
particle in text. 

barbarbarbar

[bar] -> [arb]
[arb] -> [rba]
[rba] -> [bar]


*/

// The weight is how many times the other particles follows
struct lexicon_edge {
    struct lexicon_node* to;
    int weight;
    struct lexicon_edge* next;
};

struct lexicon_node {
    char text[PARTICLE_SIZE + 1];
    int numConnections;
    struct lexicon_edge* edges;
    struct lexicon_node* next;
};
typedef struct lexicon_node Lexicon;

void Lexicon_Create(char* filename, char* outFilename, int* status);
Lexicon* Lexicon_Read(char* filename);
void Lexicon_GenerateWord(Lexicon* lex, char* buffer, int maxLength);
void Lexicon_PrintGraph(Lexicon* graph, FILE* out);

#endif