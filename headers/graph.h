#ifndef GRAPH
#define GRAPH

#include "vertex.h"
#include "edge.h"

typedef struct _graph {
	Vertex* Vertices;	// vertex list
	Edge* Edges;		// edge list
	int e_count;		// edge count
	int v_count;		// vertex count
} *Graph;

Graph Graph_Construct(int te, int tv); // te - temporary edge num
				       // tv - temporary vertex count
int Graph_LoadFile(Graph g, const char* filepath);

#endif
