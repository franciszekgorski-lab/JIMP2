#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

Graph Graph_Construct(int te, int tv) {
	Graph g = malloc(sizeof(Graph));
	
	g->e_count = te;
	g->v_count = tv;

	for (int i = 0; i < te; i++) {
		g->Edges[i] = Edge_Construct(&g->e_count);
	}

	for (int i = 0; i < tv; i++) {
		g->Vertices[i] = Vertex_Construct(&g->v_count);
	}

	return g;
}

int Graph_LoadFile(Graph g, const char* filepath) {
	FILE* file;
	if ( (file = fopen(filepath, "r")) == NULL ) {
		return 1;
	}

	int id;
	int A;
	int B;
	float w;

	while ( fscanf(file, "%d %d %d %f", &id, &A, &B, &w) == 4) {
		g->Vertices[A] = g->Edges[id]->A;
		g->Vertices[B] = g->Edges[id]->B;
		g->Edges[id]->weight = w;
	}

	return 0;
}
