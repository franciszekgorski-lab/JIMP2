#ifndef EDGE
#define EDGE

#include "vertex.h"

typedef struct _edge {
	Vertex A;
	Vertex B;
	float weight;
	int id;
} *Edge;

Edge Edge_Construct(int* id_assign);

#endif
